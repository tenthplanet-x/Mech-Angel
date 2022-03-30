#ifndef _LC_TOOLTIP_BASE_H
#define _LC_TOOLTIP_BASE_H

#include "pme.h"
#include "LC_PlayerBase.h"

namespace GameLogic
{
	//tooltip中使用的所有字段定义
	typedef enum
	{
		//普通物品
		TOOLTIP_FIELD_ITEM_ICON = 0,							//物品图标
		TOOLTIP_FIELD_ITEM_NAME = 1,							//物品名称
		TOOLTIP_FIELD_ITEM_LEVEL = 2,							//物品等级
		TOOLTIP_FIELD_ITEM_BIND = 3,							//绑定性质
		TOOLTIP_FIELD_ITEM_SUBCATEGORY = 4,						//物品子类型
		TOOLTIP_FIELD_ITEM_OWN_UNIQUE = 5,						//拥有唯一
		TOOLTIP_FIELD_ITEM_DECOMPOSITION = 6,					//可分解

		TOOLTIP_FIELD_ITEM_OVERLAP_COUNT = 7,					//物品叠加数量
		TOOLTIP_FIELD_ITEM_ORIGIN = 8,							//物品来源
		
		TOOLTIP_FIELD_ITEM_REMAIN_TIME = 9,						//剩余时间
		TOOLTIP_FIELD_ITEM_USE_COUNT = 10,						//使用次数
		
		TOOLTIP_FIELD_ITEM_LEVEL_LIMIT = 11,						//等级限制
		TOOLTIP_FIELD_ITEM_SEX_LIMIT = 12,						//性别限制
		TOOLTIP_FIELD_ITEM_WEAPON_LIMIT = 13,					//武器限制
		
		TOOLTIP_FIELD_ITEM_USE_RESULT = 14,						//使用效果
		TOOLTIP_FIELD_ITEM_PRICE = 15,							//价格
		
		TOOLTIP_FIELD_ITEM_DESCRIPT = 16,						//物品描述
		TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT = 17,					//物品氛围

		//装备类物品
		TOOLTIP_FIELD_ITEM_SELF_EQUIP_HIT = 18,					//自身装备提示
		TOOLTIP_FIELD_ITEM_QUENCH_COUNT = 19,					//强化次数
		TOOLTIP_FIELD_ITEM_QUALITY = 20,							//品质
		TOOLTIP_FIELD_ITEM_EQUIP_ATTRI = 21,						//装备基本属性
		TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI = 22,					//装备随机属性
		TOOLTIP_FIELD_ITEM_QUENCH_ATTRI = 23,					//强化属性
		TOOLTIP_FIELD_ITEM_GANG_LIMIT = 24,						//流派限制
		
		TOOLTIP_FIELD_ITEM_UPDATE = 25,							//强化
		TOOLTIP_FIELD_ITEM_MOSAIC = 26,							//镶嵌孔数
		TOOLTIP_FIELD_ITEM_SUITE_ATTRI = 27,						//套装属性
		TOOLTIP_FIELD_ITEM_SUITE_NAME = 28,						//套装名称
		TOOLTIP_FIELD_ITEM_TEXT_TITLE = 29,						//刻字
		TOOLTIP_FIELD_ITEM_TEXT_ATTRIBUTE_1 = 30,				//刻字1属性
		TOOLTIP_FIELD_ITEM_TEXT_ATTRIBUTE_2 = 31,				//刻字1属性
		TOOLTIP_FIELD_ITEM_TEXT_ATTRIBUTE_3 = 32,				//刻字1属性
		TOOLTIP_FIELD_ITEM_TEXT_ATTRIBUTE_4 = 33,				//刻字1属性
		TOOLTIP_FIELD_ITEM_APPEND_ATTRIBUTE = 34,				//附魔属性
		TOOLTIP_FIELD_ITEM_AMULET_ATTRIBUTE = 35,				//护身符属性

		TOOLTIP_FIELD_ITEM_ENDURANCE_VALUE = 36,					//耐久度

		TOOLTIP_FIELD_ITEM_HOBBY = 37,							//爱好

		//技能书类物品
		TOOLTIP_FIELD_ITEM_SKILL_BOOK_LEARNED = 38,				//技能学会属性
		TOOLTIP_FIELD_ITEM_SKILL_BOOK_LIMIT = 39,				//技能学习限制属性
		TOOLTIP_FIELD_ITEM_SKILL_BOOK_MATERIAL_REQUIRED = 40,	//技能学习材料限制属性

		//配方类物品
		TOOLTIP_FIELD_ITEM_FORMULA_LEARNED = 41,					//配方类物品的生活技能限制
		TOOLTIP_FIELD_ITEM_LIFE_SKILL_LIMIT = 42,				//配方类物品的生活技能限制
		TOOLTIP_FIELD_ITEM_MATERIAL_REQUIRED = 43,				//配方类物品的材料需求属性

		//材料类物品
		TOOLTIP_FIELD_ITEM_USE = 44,								//材料类物品的用途

		//技能学习
		TOOLTIP_FIELD_SKILL_POSITIVE = 45,						//主动or被动技能
		TOOLTIP_FIELD_SKILL_BASIC_RESULT = 46,					//基本功效果
		TOOLTIP_FIELD_SKILL_KEEP_TIME = 47,						//施展时间
		TOOLTIP_FIELD_SKILL_MP_CONSUME = 48,						//内功消耗
		TOOLTIP_FIELD_SKILL_DETAIL_DESCRIPT = 49,				//技能描述
		TOOLTIP_FIELD_SKILL_TRIGGER_RESULT = 50,					//引发效果
		TOOLTIP_FIELD_SKILL_TRIGGER_MORE_RESULT = 51,			//引发追加效果
		TOOLTIP_FIELD_SKILL_NEXE_LEVEL = 52,						//下一层
		TOOLTIP_FIELD_SKILL_STUDY_CONDITION_LIMIT = 53,			//学习条件
		TOOLTIP_FIELD_SKILL_STUDY_EXPERIENCE_LIMIT = 54,			//技能经验

		//技能
		TOOLTIP_FIELD_SKILL_NAME = 55,							//技能名称
		TOOLTIP_FIELD_SKILL_LEVEL = 56,							//技能级别
		TOOLTIP_FIELD_SKILL_DESCRIPT = 57,						//技能描述
		TOOLTIP_FIELD_SKILL_CATEGORY = 58,						//技能类型
		TOOLTIP_FIELD_SKILL_CURLEVEL = 59,						//技能当前等级
		TOOLTIP_FIELD_SKILL_LIMIT = 60,							//技能限制
		TOOLTIP_FIELD_SKILL_CONSUME = 61,						//技能消耗
		//TOOLTIP_FIELD_SKILL_COST_BERSERK = 62,					//消耗怒气
		TOOLTIP_FIELD_SKILL_EXECUTE_DISTANCE = 63,				//技能施放距离
		TOOLTIP_FIELD_SKILL_EXECUTE_TIME = 64,					//技能施放时间
		TOOLTIP_FIELD_SKILL_CD_TIME = 65,						//技能冷却时间
		TOOLTIP_FIELD_SKILL_MATERIAL_REQUIRED = 66,				//技能物品需求
		TOOLTIP_FIELD_SKILL_USE_RESULT = 67,						//技能使用效果描述
		TOOLTIP_FIELD_SKILL_USE_DESCRIPT = 68,					//技能用法介绍
		TOOLTIP_FIELD_SKILL_ATMOS_DESCRIPT = 69,					//技能氛围介绍
		TOOLTIP_FIELD_SKILL_NEXT_LEVEL = 70,						//技能升级后变化
		TOOLTIP_FIELD_SKILL_ACHIEVE = 71,						//技能成就
		TOOLTIP_FIELD_SKILL_OBSERVER_LEVEL = 72,					//观察者当前技能等级
		TOOLTIP_FIELD_SKILL_GUILDSKILL_RESEARCHDECL = 73,		//门派技能研究需要说明
		TOOLTIP_FIELD_SKILL_GUILDSKILL_RESEARCHSTATE = 74,		//门派技能研究状态
		TOOLTIP_FIELD_SKILL_POSNEG_TYPE = 75,					//技能主被动类型
		TOOLTIP_FIELD_SKILL_PLAYER_LEVEL = 76,					//技能需要达到的人物等级
		TOOLTIP_FIELD_SKILL_UNLOCK_LEVEL = 81,					//技能解锁等级

		TOOLTIP_FIELD_SKILL_UPDATE_NEEDEXP = 77,					//技能升级需要的经验
		TOOLTIP_FIELD_SKILL_UPDATE_NEEDCASH = 78,				//技能升级需要的金钱
		TOOLTIP_FIELD_SKILL_UPDATE_NEEDLEVEL = 79,				//技能升级需要的等级

		//技能状态
		TOOLTIP_FIELD_SKILLSTATE_USE_RESULT = 80,				//技能状态使用效果
		TOOLTIP_FIELD_EQUIP_SKILL			 = 82,				//装备技能

		//技能状态
		//TOOLTIP_FIELD_TALENT_NAME = 81,							//天赋名字
		//TOOLTIP_FIELD_TALENT_CATEGORY = 82,						//天赋名字
		//TOOLTIP_FIELD_TALENT_FACTION = 83,						//天赋派系
		//TOOLTIP_FIELD_TALENT_NEED_SUPREMACY_LEVEL = 84,			//需求至尊等级
		//TOOLTIP_FIELD_TALENT_LEVEL = 85,							//天赋等级
		//TOOLTIP_FIELD_TALENT_NEED_TALENT_POINT = 86,				//需求天赋点
		//TOOLTIP_FIELD_TALENT_NEED_PRE_TALENT = 87,				//需求前置天赋
		//TOOLTIP_FIELD_TALENT_NEED_PRE_TALENT_POINT = 88,			//需求前置天赋点
		//TOOLTIP_FIELD_TALENT_USE_RESULT = 89,					//使用效果
		//TOOLTIP_FIELD_TALENT_ATMOS_DESCRIPT = 90,				//氛围描述
		//TOOLTIP_FIELD_TALENT_OPERATION_TIP = 91,					//氛围描述

		//TOOLTIP_FIELD_TALENT_DEPEND,						//前置天赋
		//TOOLTIP_FIELD_TALENT_RESULT,						//天赋效果
		//TOOLTIP_FIELD_TALENT_FORMULA_RESULT,				//口诀效果

		
		TOOLTIP_FIELD_COMPARE_DESCRIPT = 92,						//装备属性比较数值
		TOOLTIP_FIELD_COMPARE_DESCRIPT_NAME = 93,				//装备属性比较名称
		//TOOLTIP_FIELD_COMPARE_SPLIT_LINE = 94,					//装备属性比较分割线
		TOOLTIP_FIELD_COMPARE_CURRENT_GS = 95,					//当前战斗力
		TOOLTIP_FIELD_COMPARE_MAX_GS = 96,						//强化顶级战斗力
		TOOLTIP_FIELD_COMPARE_MAX_GS_DIFF = 97,					//强化顶级战斗力差
		
		//门派限制
		TOOLTIP_FIELD_ITEM_SCHOOL_LIMIT = 98,

		//打坐
		TOOLTIP_FIELD_MEDITATE_TITLE = 99,						//打坐标题
		TOOLTIP_FIELD_MEDITATE_TIME = 100,						//打坐累计时间
		TOOLTIP_FIELD_MEDITATE_SKILLPOINT_LEVEL = 101,			//真气等级
		TOOLTIP_FIELD_MEDITATE_SKILLPOINT_GAINED = 102,			//获得真气
		TOOLTIP_FIELD_MEDITATE_TOTAL_SKILLPOINT = 103,			//真气总值
		TOOLTIP_FIELD_MEDITATE_EXP_GAINED = 127,			//获得经验

		TOOLTIP_FIELD_ITEM_DETAIL_DESCRIPT = 104,				//物品详细描述
		TOOLTIP_FIELD_ITEM_UNIDENTIFY_TIP = 105,				//物品未鉴定提示
		TOOLTIP_FIELD_ITEM_EQUIP_SLOT_ATTR = 106,				//装备镶嵌信息
		TOOLTIP_FIELD_ITEM_EQUIP_EMPTYSLOT = 107,				//空的镶嵌孔
		TOOLTIP_FIELD_ITEM_BOTTOM_UNIDENTIFY = 108,				//底部未鉴定提示
		TOOLTIP_FIELD_ITEM_SPLIT_TIP = 109,						//拆分提示
		TOOLTIP_FIELD_ITEM_SOCKET_TIP = 110,					//镶嵌提示

		//活动
		TOOLTIP_FIELD_GAMESTORY_NAME				= 111,			//活动名称
		TOOLTIP_FIELD_GAMESTORY_PLAY_RULE_TITLE		= 112,			//玩法说明标题
		TOOLTIP_FIELD_GAMESTORY_PLAY_RULE			= 113,			//玩法说明内容
		TOOLTIP_FIELD_GAMESTORY_REWARD_TITLE		= 114,			//携带宝物标题
		TOOLTIP_FIELD_GAMESTORY_REWARD				= 115,			//携带宝物内容
		TOOLTIP_FIELD_GAMESTORY_RESET_TIME_TITLE	= 116,			//刷新时间标题
		TOOLTIP_FIELD_GAMESTORY_RESET_TIME			= 117,			//刷新时间内容
		TOOLTIP_FIELD_GAMESTORY_RESET_MAP_TITLE		= 118,			//刷新地图标题
		TOOLTIP_FIELD_GAMESTORY_RESET_MAP			= 119,			//刷新地图内容
		TOOLTIP_FIELD_GAMESTORY_TIP					= 120,			//注意事项
		TOOLTIP_FIELD_GAMESTORY_BOOS_NAME			= 121,			//boss名字
		TOOLTIP_FIELD_GAMESTORY_BOOS_LEVEL			= 122,			//boss等级
		TOOLTIP_FIELD_GAMESTORY_BOOS_MAP_TITLE		= 123,			//驻守地图标题
		TOOLTIP_FIELD_GAMESTORY_BOOS_MAP			= 124,			//驻守地图
		TOOLTIP_FIELD_GAMESTORY_BOOS_BASE_ATTR_TITLE= 125,			//基本属性标题
		TOOLTIP_FIELD_GAMESTORY_BOOS_BASE_ATTR		= 126,			//基本属性

		TOOLTIP_FIELD_ITEM_EQUIP_REFINE_ATTRIB		= 127,			//装备已洗练属性

		TOOLTIP_FIELD_ITEM_EQUIP_TIP				= 128,			//装备提示
		TOOLTIP_FIELD_ITEM_SELL_TIP					= 129,			//出售提示
		TOOLTIP_FIELD_ITEM_VIDEO					= 130,			//视频
		TOOLTIP_FIELD_ITEM_REINFORCE				= 131,			//强化

		TOOLTIP_FIELD_ITEM_GUILD_CASH				= 132,			//公会贡献
		TOOLTIP_FIELD_ITEM_EXPIRE_TIME				= 133,			//限时道具倒计时
		TOOLTIP_FIELD_ITEM_BATCH_USE				= 134,			//批量使用

		TOOLTIP_FIELD_ITEM_BASE_SCORE				= 135,			//基础战斗力

		TOOLTIP_FIELD_SKILLBOOK_SCORE				= 136,			//秘籍战斗力
		TOOLTIP_FIELD_SKILLBOOK_ATTRIB				= 137,			//秘籍属性
		TOOLTIP_FIELD_SKILLBOOK_DES					= 138,			//秘籍描述
		TOOLTIP_FIELD_AVATAR						= 139,			//avatar
		TOOLTIP_FIELD_TRANSFORMCARD_DES				= 140,			//秘籍描述

		TOOLTIP_FIELD_ADDITEM_LINKINFO				= 141,			//道具link信息
		TOOLTIP_FIELD_ITEM_EQUIP_SUPER_ATTRI		= 142,
		TOOLTIP_FIELD_ITEM_EQUIP_TOTEM_ATTRI		= 143,

		TOOLTIP_FIELD_ITEM_EQUIP_GEM_BUFF			= 144,			//宝石加持
		TOOLTIP_FIELD_ITEM_EQUIP_SLOT_ICON			= 145,			//装备镶嵌图标

		TOOLTIP_FIELD_SKILLBOOK_SKILLINFO			= 146,			//秘籍技能描述
		TOOLTIP_FIELD_SKILLBOOK_TALENTINFO			= 147,			//秘籍天赋描述
		TOOLTIP_FIELD_SKILLBOOK_STARINFO			= 148,			//秘籍初始星级描述

	} TooltipFieldType;

	//字体
	typedef enum
	{
		FONT_SMALL			= 0,							//12号字
		FONT_MIDDLE			= 1,							//15号字
		FONT_LARGE			= 3,							//20号字
	} TooltipFieldFont;
	//字体大小
	typedef enum
	{
		FONTSIZE_XSMALL		= 0,
		FONTSIZE_SMALL		= 1,							//小号
		FONTSIZE_MIDDLE		= 2,							//中号	
		FONTSIZE_LARGE		= 3,							//大号
	} TooltipFieldFontSize;
	//字体颜色
	typedef enum
	{
		//WHITE		= 0xFFFFFFFF,							//白色
		//BLUE		= 0xFF0000FF,							//蓝色
		//GRAY		= 0xFFC0C0C0,							//灰色
		//GOLDENROD	= 0xFFDAA520,							//暗金
		//RED			= 0xFFFF0000,							//红色
		//GOLD		= 0xFFFFD700,							//金色
		//GREEN		= 0xFF00FF00,							//绿色

		//根据美术定义的颜色重定义宏
		WHITE		= 0xffffffff,							//白色
		WHITE2		= 0xffcecece,							//白色
		YELLOW		= 0xffffea3c,							//黄色
		PURPLE		= 0xffb214e6,							//紫色
		BLUE		= 0xff02c4ff,							//蓝色
		GRAY		= 0xff7d7d7d,							//灰色
		GRAY2		= 0xff858585,							//灰色2
		DARKGRAY	= 0xff6e6d6d,							//深灰色
		GOLDENROD	= 0xffbe8247,							//暗金
		RED			= 0xffff0000,							//红色
		GOLD		= 0xffe2b54a,							//金色
		GOLD2		= 0xffc7b377,							//金色2
		GREEN		= 0xff00FF00,							//绿色
		PINK		= 0xc4a2a0,								//粉色
		ORANGE		= 0xdbbd9b,								//橙色
		DARKORANGE	= 0xcd9a65,								//暗橙色
		NEWGOLD		= 0xffbe8247,
		BROWN		= 0xffbc863f,							//棕色
		PURPLE2		= 0xff9e2bff,
		PURPLE3		= 0xffcc00ff,
		BROWN2		= 0xffcebe67,		

	} TooltipFieldColor;

	class LC_TooltipField : public Memory::MM_BaseObject
	{
	public:
		LC_TooltipField(void);
		LC_TooltipField(TooltipFieldType	kFieldType,
						const StringType&	kCaption,
						bool				bEnable,
						const StringType&	kContent, 
						unsigned short		usFont			= FONT_MIDDLE,
						unsigned short		usFontSize		= FONTSIZE_MIDDLE,
						int					iFontColor		= WHITE, 
						unsigned short		usLengthLimit	= 0,
						unsigned short		usEntryNumLimit = 0,
						bool				bNewLine		= true);
		~LC_TooltipField(void);

		//MEMBER_UNSHORT(Type);							//字段类型
		MEMBER_STRING(Caption);							//
		MEMBER_BOOL(Enable);							//是否显示该字段
		MEMBER_STRING(Content);							//字段内容
		MEMBER_UNSHORT(Font);							//字体
		MEMBER_UNSHORT(FontSize);						//字体尺寸
		MEMBER_INT(FontColor);							//字体颜色
		MEMBER_UNSHORT(LengthLimit);					//字段长度限制
		MEMBER_UNSHORT(EntryNumLimit);					//字段条目限制
		MEMBER_BOOL(NewLine);							//是否换行
	protected:
		//union
		//{
		//	unsigned short m_usLengthLimit;				//字段内容长度限制
		//	unsigned short m_usEntryNumLimit;			//字段条目限制
		//};
		
		TooltipFieldType m_kType;						//字段类型

	public:
		void				SetType(TooltipFieldType kType){m_kType = kType;};
		TooltipFieldType	GetType(){return m_kType;};

		/*void				SetContentLengthLimit(unsigned short usLengthLimit){m_usLengthLimit = usLengthLimit;};
		unsigned short		GetContentLengthLimit(){return m_usLengthLimit;};

		void				SetContentEntryNumLimit(unsigned short usEntryNumLimit){m_usEntryNumLimit = usEntryNumLimit;};
		unsigned short		GetContentEntryNumLimit(){return m_usEntryNumLimit;};*/
	};


	class LC_BackPackEntry;
	class LC_ActorBase;
	class LC_TooltipBase : public Memory::MM_BaseObject
	{
	public:
		LC_TooltipBase(void);
		virtual ~LC_TooltipBase(void);

		typedef std::vector<LC_TooltipField> TooltipFieldVector;
		typedef Utility::UT_VectorIterator<TooltipFieldVector> TooltipFieldIterator;

		virtual void  Init(LC_PlayerBase* pkObserver);
		virtual bool  BuildTooltip() = 0;
		TooltipFieldIterator GetIterator(){return TooltipFieldIterator(m_kTooltipFieldVector.begin(), m_kTooltipFieldVector.end());}
		TooltipFieldVector&	 GetTooltipFields(){return m_kTooltipFieldVector;}

		//Begin 构造tooltip的参数接口
		void		  SetObserver(LC_PlayerBase* pkObserver){m_pkObserver = pkObserver;};
		LC_PlayerBase* GetObserver(){return m_pkObserver;};
		//End

		bool GetSkillStateUseResult(int lRecordID, StringType &kUseResult);

	protected:
		//Begin 字符串处理
		StringType _numArabToChinese(int lArabNum);					//将阿拉伯数字转换成汉字
		bool	   _formatTooltip(TooltipFieldVector& kFieldVector);	//格式化Tooltip的显示格式（在UI完善以前暂时使用该功能）

		int			_variable2Value(const StringType& kMemberName, int lRecordID);
		//End
		
		//解析文本获取物品的使用效果
		template<typename T>
		bool _getUseResult(int lRecordID, StringType& kUseResult, bool bUseResult2=false)
		{
			if (lRecordID <= 0)
			{
				return false;
			}

			typename T* pkRecordList = typename T::GetSingletonPtr();
			if (0 == pkRecordList)
			{
				return false;
			}
			typename T::DataEntry* pkRecordData = pkRecordList->GetEntryPtr(lRecordID);
			if (0 == pkRecordData)
			{
				return false;
			}

			if ( ( !bUseResult2 && pkRecordData->_kUseResult.empty()) || (bUseResult2 && pkRecordData->_kUseResult2.empty()) )
			{
				return false;
			}

			std::wstring kContent = CodeConvert::Utf8ToWcs(bUseResult2 ? pkRecordData->_kUseResult2 : pkRecordData->_kUseResult);		

			//按照ungreedy方式进行匹配
			PME kPattern("(.+)[\\+\\-\\*\\/](.+)", "giU");

			std::wstringstream kStringStream;

			std::wstring::size_type stParamStart = std::wstring::npos;
			std::wstring kPostfix = kContent;
			while ( (stParamStart = kContent.find('[')) != std::wstring::npos )
			{
				std::wstring::size_type stParamEnd = kContent.find(']');
				std::wstring kPrefix = kContent.substr(0, stParamStart);
				std::wstring kParams = kContent.substr(stParamStart + 1, stParamEnd - stParamStart - 1);
				kPostfix = kContent.substr(stParamEnd + 1, kContent.length() - stParamEnd);
				kContent = kPostfix;									//继续查找后面的[]


				int iParamStart = 0;
				TStringVector kParamVector;
				TStringVector kOperatorVector;
				//获取所有参与运算的数字和运算符
				while ( kPattern.match(TPSTR2STDSTR(CodeConvert::WcsToUtf8(kParams))) > 0 )
				{
					PME::markers kParam1Pos = kPattern.GetMatchPos(1);
					PME::markers kParam2Pos = kPattern.GetMatchPos(2);

					int iOperatorStart = kParam1Pos.second;
					int iOperatorEnd = kParam2Pos.first;
					StringType kOperator = CodeConvert::WcsToUtf8(kParams.substr(iOperatorStart, iOperatorEnd - iOperatorStart));
					StringType kParam1 = CodeConvert::WcsToUtf8(kParams.substr(iParamStart, iOperatorStart - iParamStart));
					
					iParamStart = iOperatorEnd;
					
					kParamVector.push_back(kParam1);					//保存运算数
					UT_BaseStringUtility::Trim(kOperator);
					kOperatorVector.push_back( kOperator );		//保存运算符
				}
				//保存最后一个运算数
				StringType kLastParam = CodeConvert::WcsToUtf8(kParams.substr(iParamStart, kParams.length() - iParamStart));
				kParamVector.push_back(kLastParam);


				//进行四则运算
				int iParamCount = kParamVector.size();
				int iOperatorCount = kOperatorVector.size();
				double fParamResult = 0;
				//如果只有一个参数，即没有运算符
				if (iParamCount == 1 && iOperatorCount <= 0)
				{
					const StringType & kParam = kParamVector[0];
					//如果输入的是变量名，在csv表中寻找对应的数值
					if (atof(kParam.c_str()) - 0 < FLT_EPSILON)
					{
						const StringType & kMemberName( kParam );
						StringType kMemberType = "";
						StringType kMemberData = "";
						fParamResult = pkRecordList->DecodeNumMember(lRecordID, kMemberName, kMemberType, kMemberData);
					}
					else	//如果输入的是数值
					{
						fParamResult = atof(kParam.c_str());
					}	
				}
				else if (iOperatorCount >= 1)	//如果运算符存在
				{
					if (iParamCount > 0)
					{
						//取第一个运算数
						const StringType & kParam = kParamVector[0];
						//如果输入的是变量名，在csv表中寻找对应的数值
						if (atof(kParam.c_str()) - 0 < FLT_EPSILON)
						{
							const StringType & kMemberName(kParam);
							StringType kMemberType;
							StringType kMemberData;
							fParamResult = pkRecordList->DecodeNumMember(lRecordID, kMemberName, kMemberType, kMemberData);
							if(-1 == fParamResult)
							{
								//坐骑技能状态特殊处理
// 								if("VLevel" == kMemberName)
// 								{
// 									LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
// 									if(NULL != pkPlayer)
// 									{
// 										fParamResult = pkPlayer->GetVehicleCurrentLevel();
// 									}
// 								}
// 								else if ("VCurStateLevel" == kMemberName)
// 								{
// 									LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
// 									LC_ActorStateMap* stateMap = pkPlayer->GetStateMap();
// 									LC_ActorStateEntry* pk = stateMap->GetStateEntryByID((int)lRecordID);
// 									if (pk)
// 									{
// 										fParamResult = pk->GetOverlapCount();
// 									}
// 								}
// 								else if ("VLevelSpeedDelta" == kMemberName)
// 								{
// 									LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
// 									if (pkPlayer)
// 										fParamResult = pkPlayer->GetVehicleCurrentSpeedDelta();
// 								}
// 								else if ("GuildWarExp" == kMemberName)
// 								{
// 									LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
// 									if (pkPlayer && pkPlayer->GetGuildWarAssert())
// 									{
// 										LC_GuildWarAssert* asset = pkPlayer->GetGuildWarAssert();
// 										int level = pkPlayer->GetLevel();
// 										fParamResult = (level * 5 + 20) * 5 * asset->GetMultiple();
// 									}
// 								}
								fParamResult = _variable2Value(kMemberName, lRecordID);
							}
						}
						else	//如果输入的是数值
						{
							fParamResult = atof(kParam.c_str());
						}
					}
				}

				for (int i = 0; i < iOperatorCount; i ++)
				{
					if (i + 1 > iParamCount - 1)
					{
						break;
					}
					const StringType & kParam = kParamVector[i + 1];
					double fParam = 0;
					//如果输入的是变量名，在csv表中寻找对应的数值
					if (atof(kParam.c_str()) - 0 < FLT_EPSILON)
					{
						const StringType & kMemberName( kParam );
						StringType kMemberType;
						StringType kMemberData;
						fParam = pkRecordList->DecodeNumMember(lRecordID, kMemberName, kMemberType, kMemberData);
						if(-1 == fParam)
						{
							//坐骑技能状态特殊处理
// 							if("VLevel" == kMemberName)
// 							{
// 								LC_PlayerBase *pkPlayer = (LC_PlayerBase *)GetObserver();
// 								if(NULL != pkPlayer)
// 								{
// 									fParam = pkPlayer->GetVehicleCurrentLevel();
// 								}
// 							}
// 							else if ("VCurStateLevel" == kMemberName)
// 							{
// 								LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
// 								LC_ActorStateMap* stateMap = pkPlayer->GetStateMap();
// 								LC_ActorStateEntry* pk = stateMap->GetStateEntryByID((int)lRecordID);
// 								if (pk)
// 								{
// 									fParamResult = pk->GetOverlapCount();
// 								}
// 							}
// 							else if ("VLevelSpeedDelta" == kMemberName)
// 							{
// 								LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
// 								fParamResult = pkPlayer->GetVehicleCurrentSpeedDelta();
// 							}
// 							else if ("GuildWarExp" == kMemberName)
// 							{
// 								LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
// 								if (pkPlayer && pkPlayer->GetGuildWarAssert())
// 								{
// 									LC_GuildWarAssert* asset = pkPlayer->GetGuildWarAssert();
// 									int level = pkPlayer->GetLevel();
// 									fParamResult = (level * 5 + 20) * 5 * asset->GetMultiple();
// 								}
// 							}
							fParamResult = _variable2Value(kMemberName, lRecordID);
						}
					}
					else	//如果输入的是数值
					{
						fParam = atof(kParam.c_str());
					}

					const StringType & kOperator = kOperatorVector[i];
					if (kOperator == "+")
					{
						fParamResult += fParam;
					}
					else if (kOperator == "-")
					{
						fParamResult -= fParam;
					}
					else if (kOperator == "*")
					{
						fParamResult *= fParam;
					}
					else if (kOperator == "/")
					{
						fParamResult /= fParam;
					}
				}
				
				kStringStream << kPrefix << "<color val=0xff22ce4e>";					//数值前的文字部分
				long ts = Utility::Double2Long(fParamResult);
				if ( fParamResult - ts >= 0.1f)
				{
					kStringStream << fixed << setprecision(1) << fParamResult << "</color>" ;		//数值部分，取整处理
				}
				else
				{
					kStringStream << ts << "</color>";		//数值部分，取整处理
				}
			}

			kStringStream << kPostfix;
			kUseResult = CodeConvert::WcsToUtf8(kStringStream.str());

			return true;
		}

		
	private:
		TooltipFieldVector m_kTooltipFieldVector;		//保存tooltip数据
		LC_PlayerBase*	   m_pkObserver;				//tooltip观察者
	};
}

#endif

