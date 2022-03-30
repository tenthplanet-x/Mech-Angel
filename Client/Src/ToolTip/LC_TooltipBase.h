#ifndef _LC_TOOLTIP_BASE_H
#define _LC_TOOLTIP_BASE_H

#include "pme.h"
#include "LC_PlayerBase.h"

namespace GameLogic
{
	//tooltip��ʹ�õ������ֶζ���
	typedef enum
	{
		//��ͨ��Ʒ
		TOOLTIP_FIELD_ITEM_ICON = 0,							//��Ʒͼ��
		TOOLTIP_FIELD_ITEM_NAME = 1,							//��Ʒ����
		TOOLTIP_FIELD_ITEM_LEVEL = 2,							//��Ʒ�ȼ�
		TOOLTIP_FIELD_ITEM_BIND = 3,							//������
		TOOLTIP_FIELD_ITEM_SUBCATEGORY = 4,						//��Ʒ������
		TOOLTIP_FIELD_ITEM_OWN_UNIQUE = 5,						//ӵ��Ψһ
		TOOLTIP_FIELD_ITEM_DECOMPOSITION = 6,					//�ɷֽ�

		TOOLTIP_FIELD_ITEM_OVERLAP_COUNT = 7,					//��Ʒ��������
		TOOLTIP_FIELD_ITEM_ORIGIN = 8,							//��Ʒ��Դ
		
		TOOLTIP_FIELD_ITEM_REMAIN_TIME = 9,						//ʣ��ʱ��
		TOOLTIP_FIELD_ITEM_USE_COUNT = 10,						//ʹ�ô���
		
		TOOLTIP_FIELD_ITEM_LEVEL_LIMIT = 11,						//�ȼ�����
		TOOLTIP_FIELD_ITEM_SEX_LIMIT = 12,						//�Ա�����
		TOOLTIP_FIELD_ITEM_WEAPON_LIMIT = 13,					//��������
		
		TOOLTIP_FIELD_ITEM_USE_RESULT = 14,						//ʹ��Ч��
		TOOLTIP_FIELD_ITEM_PRICE = 15,							//�۸�
		
		TOOLTIP_FIELD_ITEM_DESCRIPT = 16,						//��Ʒ����
		TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT = 17,					//��Ʒ��Χ

		//װ������Ʒ
		TOOLTIP_FIELD_ITEM_SELF_EQUIP_HIT = 18,					//����װ����ʾ
		TOOLTIP_FIELD_ITEM_QUENCH_COUNT = 19,					//ǿ������
		TOOLTIP_FIELD_ITEM_QUALITY = 20,							//Ʒ��
		TOOLTIP_FIELD_ITEM_EQUIP_ATTRI = 21,						//װ����������
		TOOLTIP_FIELD_ITEM_EQUIP_RANDOM_ATTRI = 22,					//װ���������
		TOOLTIP_FIELD_ITEM_QUENCH_ATTRI = 23,					//ǿ������
		TOOLTIP_FIELD_ITEM_GANG_LIMIT = 24,						//��������
		
		TOOLTIP_FIELD_ITEM_UPDATE = 25,							//ǿ��
		TOOLTIP_FIELD_ITEM_MOSAIC = 26,							//��Ƕ����
		TOOLTIP_FIELD_ITEM_SUITE_ATTRI = 27,						//��װ����
		TOOLTIP_FIELD_ITEM_SUITE_NAME = 28,						//��װ����
		TOOLTIP_FIELD_ITEM_TEXT_TITLE = 29,						//����
		TOOLTIP_FIELD_ITEM_TEXT_ATTRIBUTE_1 = 30,				//����1����
		TOOLTIP_FIELD_ITEM_TEXT_ATTRIBUTE_2 = 31,				//����1����
		TOOLTIP_FIELD_ITEM_TEXT_ATTRIBUTE_3 = 32,				//����1����
		TOOLTIP_FIELD_ITEM_TEXT_ATTRIBUTE_4 = 33,				//����1����
		TOOLTIP_FIELD_ITEM_APPEND_ATTRIBUTE = 34,				//��ħ����
		TOOLTIP_FIELD_ITEM_AMULET_ATTRIBUTE = 35,				//���������

		TOOLTIP_FIELD_ITEM_ENDURANCE_VALUE = 36,					//�;ö�

		TOOLTIP_FIELD_ITEM_HOBBY = 37,							//����

		//����������Ʒ
		TOOLTIP_FIELD_ITEM_SKILL_BOOK_LEARNED = 38,				//����ѧ������
		TOOLTIP_FIELD_ITEM_SKILL_BOOK_LIMIT = 39,				//����ѧϰ��������
		TOOLTIP_FIELD_ITEM_SKILL_BOOK_MATERIAL_REQUIRED = 40,	//����ѧϰ������������

		//�䷽����Ʒ
		TOOLTIP_FIELD_ITEM_FORMULA_LEARNED = 41,					//�䷽����Ʒ�����������
		TOOLTIP_FIELD_ITEM_LIFE_SKILL_LIMIT = 42,				//�䷽����Ʒ�����������
		TOOLTIP_FIELD_ITEM_MATERIAL_REQUIRED = 43,				//�䷽����Ʒ�Ĳ�����������

		//��������Ʒ
		TOOLTIP_FIELD_ITEM_USE = 44,								//��������Ʒ����;

		//����ѧϰ
		TOOLTIP_FIELD_SKILL_POSITIVE = 45,						//����or��������
		TOOLTIP_FIELD_SKILL_BASIC_RESULT = 46,					//������Ч��
		TOOLTIP_FIELD_SKILL_KEEP_TIME = 47,						//ʩչʱ��
		TOOLTIP_FIELD_SKILL_MP_CONSUME = 48,						//�ڹ�����
		TOOLTIP_FIELD_SKILL_DETAIL_DESCRIPT = 49,				//��������
		TOOLTIP_FIELD_SKILL_TRIGGER_RESULT = 50,					//����Ч��
		TOOLTIP_FIELD_SKILL_TRIGGER_MORE_RESULT = 51,			//����׷��Ч��
		TOOLTIP_FIELD_SKILL_NEXE_LEVEL = 52,						//��һ��
		TOOLTIP_FIELD_SKILL_STUDY_CONDITION_LIMIT = 53,			//ѧϰ����
		TOOLTIP_FIELD_SKILL_STUDY_EXPERIENCE_LIMIT = 54,			//���ܾ���

		//����
		TOOLTIP_FIELD_SKILL_NAME = 55,							//��������
		TOOLTIP_FIELD_SKILL_LEVEL = 56,							//���ܼ���
		TOOLTIP_FIELD_SKILL_DESCRIPT = 57,						//��������
		TOOLTIP_FIELD_SKILL_CATEGORY = 58,						//��������
		TOOLTIP_FIELD_SKILL_CURLEVEL = 59,						//���ܵ�ǰ�ȼ�
		TOOLTIP_FIELD_SKILL_LIMIT = 60,							//��������
		TOOLTIP_FIELD_SKILL_CONSUME = 61,						//��������
		//TOOLTIP_FIELD_SKILL_COST_BERSERK = 62,					//����ŭ��
		TOOLTIP_FIELD_SKILL_EXECUTE_DISTANCE = 63,				//����ʩ�ž���
		TOOLTIP_FIELD_SKILL_EXECUTE_TIME = 64,					//����ʩ��ʱ��
		TOOLTIP_FIELD_SKILL_CD_TIME = 65,						//������ȴʱ��
		TOOLTIP_FIELD_SKILL_MATERIAL_REQUIRED = 66,				//������Ʒ����
		TOOLTIP_FIELD_SKILL_USE_RESULT = 67,						//����ʹ��Ч������
		TOOLTIP_FIELD_SKILL_USE_DESCRIPT = 68,					//�����÷�����
		TOOLTIP_FIELD_SKILL_ATMOS_DESCRIPT = 69,					//���ܷ�Χ����
		TOOLTIP_FIELD_SKILL_NEXT_LEVEL = 70,						//����������仯
		TOOLTIP_FIELD_SKILL_ACHIEVE = 71,						//���ܳɾ�
		TOOLTIP_FIELD_SKILL_OBSERVER_LEVEL = 72,					//�۲��ߵ�ǰ���ܵȼ�
		TOOLTIP_FIELD_SKILL_GUILDSKILL_RESEARCHDECL = 73,		//���ɼ����о���Ҫ˵��
		TOOLTIP_FIELD_SKILL_GUILDSKILL_RESEARCHSTATE = 74,		//���ɼ����о�״̬
		TOOLTIP_FIELD_SKILL_POSNEG_TYPE = 75,					//��������������
		TOOLTIP_FIELD_SKILL_PLAYER_LEVEL = 76,					//������Ҫ�ﵽ������ȼ�
		TOOLTIP_FIELD_SKILL_UNLOCK_LEVEL = 81,					//���ܽ����ȼ�

		TOOLTIP_FIELD_SKILL_UPDATE_NEEDEXP = 77,					//����������Ҫ�ľ���
		TOOLTIP_FIELD_SKILL_UPDATE_NEEDCASH = 78,				//����������Ҫ�Ľ�Ǯ
		TOOLTIP_FIELD_SKILL_UPDATE_NEEDLEVEL = 79,				//����������Ҫ�ĵȼ�

		//����״̬
		TOOLTIP_FIELD_SKILLSTATE_USE_RESULT = 80,				//����״̬ʹ��Ч��
		TOOLTIP_FIELD_EQUIP_SKILL			 = 82,				//װ������

		//����״̬
		//TOOLTIP_FIELD_TALENT_NAME = 81,							//�츳����
		//TOOLTIP_FIELD_TALENT_CATEGORY = 82,						//�츳����
		//TOOLTIP_FIELD_TALENT_FACTION = 83,						//�츳��ϵ
		//TOOLTIP_FIELD_TALENT_NEED_SUPREMACY_LEVEL = 84,			//��������ȼ�
		//TOOLTIP_FIELD_TALENT_LEVEL = 85,							//�츳�ȼ�
		//TOOLTIP_FIELD_TALENT_NEED_TALENT_POINT = 86,				//�����츳��
		//TOOLTIP_FIELD_TALENT_NEED_PRE_TALENT = 87,				//����ǰ���츳
		//TOOLTIP_FIELD_TALENT_NEED_PRE_TALENT_POINT = 88,			//����ǰ���츳��
		//TOOLTIP_FIELD_TALENT_USE_RESULT = 89,					//ʹ��Ч��
		//TOOLTIP_FIELD_TALENT_ATMOS_DESCRIPT = 90,				//��Χ����
		//TOOLTIP_FIELD_TALENT_OPERATION_TIP = 91,					//��Χ����

		//TOOLTIP_FIELD_TALENT_DEPEND,						//ǰ���츳
		//TOOLTIP_FIELD_TALENT_RESULT,						//�츳Ч��
		//TOOLTIP_FIELD_TALENT_FORMULA_RESULT,				//�ھ�Ч��

		
		TOOLTIP_FIELD_COMPARE_DESCRIPT = 92,						//װ�����ԱȽ���ֵ
		TOOLTIP_FIELD_COMPARE_DESCRIPT_NAME = 93,				//װ�����ԱȽ�����
		//TOOLTIP_FIELD_COMPARE_SPLIT_LINE = 94,					//װ�����ԱȽϷָ���
		TOOLTIP_FIELD_COMPARE_CURRENT_GS = 95,					//��ǰս����
		TOOLTIP_FIELD_COMPARE_MAX_GS = 96,						//ǿ������ս����
		TOOLTIP_FIELD_COMPARE_MAX_GS_DIFF = 97,					//ǿ������ս������
		
		//��������
		TOOLTIP_FIELD_ITEM_SCHOOL_LIMIT = 98,

		//����
		TOOLTIP_FIELD_MEDITATE_TITLE = 99,						//��������
		TOOLTIP_FIELD_MEDITATE_TIME = 100,						//�����ۼ�ʱ��
		TOOLTIP_FIELD_MEDITATE_SKILLPOINT_LEVEL = 101,			//�����ȼ�
		TOOLTIP_FIELD_MEDITATE_SKILLPOINT_GAINED = 102,			//�������
		TOOLTIP_FIELD_MEDITATE_TOTAL_SKILLPOINT = 103,			//������ֵ
		TOOLTIP_FIELD_MEDITATE_EXP_GAINED = 127,			//��þ���

		TOOLTIP_FIELD_ITEM_DETAIL_DESCRIPT = 104,				//��Ʒ��ϸ����
		TOOLTIP_FIELD_ITEM_UNIDENTIFY_TIP = 105,				//��Ʒδ������ʾ
		TOOLTIP_FIELD_ITEM_EQUIP_SLOT_ATTR = 106,				//װ����Ƕ��Ϣ
		TOOLTIP_FIELD_ITEM_EQUIP_EMPTYSLOT = 107,				//�յ���Ƕ��
		TOOLTIP_FIELD_ITEM_BOTTOM_UNIDENTIFY = 108,				//�ײ�δ������ʾ
		TOOLTIP_FIELD_ITEM_SPLIT_TIP = 109,						//�����ʾ
		TOOLTIP_FIELD_ITEM_SOCKET_TIP = 110,					//��Ƕ��ʾ

		//�
		TOOLTIP_FIELD_GAMESTORY_NAME				= 111,			//�����
		TOOLTIP_FIELD_GAMESTORY_PLAY_RULE_TITLE		= 112,			//�淨˵������
		TOOLTIP_FIELD_GAMESTORY_PLAY_RULE			= 113,			//�淨˵������
		TOOLTIP_FIELD_GAMESTORY_REWARD_TITLE		= 114,			//Я���������
		TOOLTIP_FIELD_GAMESTORY_REWARD				= 115,			//Я����������
		TOOLTIP_FIELD_GAMESTORY_RESET_TIME_TITLE	= 116,			//ˢ��ʱ�����
		TOOLTIP_FIELD_GAMESTORY_RESET_TIME			= 117,			//ˢ��ʱ������
		TOOLTIP_FIELD_GAMESTORY_RESET_MAP_TITLE		= 118,			//ˢ�µ�ͼ����
		TOOLTIP_FIELD_GAMESTORY_RESET_MAP			= 119,			//ˢ�µ�ͼ����
		TOOLTIP_FIELD_GAMESTORY_TIP					= 120,			//ע������
		TOOLTIP_FIELD_GAMESTORY_BOOS_NAME			= 121,			//boss����
		TOOLTIP_FIELD_GAMESTORY_BOOS_LEVEL			= 122,			//boss�ȼ�
		TOOLTIP_FIELD_GAMESTORY_BOOS_MAP_TITLE		= 123,			//פ�ص�ͼ����
		TOOLTIP_FIELD_GAMESTORY_BOOS_MAP			= 124,			//פ�ص�ͼ
		TOOLTIP_FIELD_GAMESTORY_BOOS_BASE_ATTR_TITLE= 125,			//�������Ա���
		TOOLTIP_FIELD_GAMESTORY_BOOS_BASE_ATTR		= 126,			//��������

		TOOLTIP_FIELD_ITEM_EQUIP_REFINE_ATTRIB		= 127,			//װ����ϴ������

		TOOLTIP_FIELD_ITEM_EQUIP_TIP				= 128,			//װ����ʾ
		TOOLTIP_FIELD_ITEM_SELL_TIP					= 129,			//������ʾ
		TOOLTIP_FIELD_ITEM_VIDEO					= 130,			//��Ƶ
		TOOLTIP_FIELD_ITEM_REINFORCE				= 131,			//ǿ��

		TOOLTIP_FIELD_ITEM_GUILD_CASH				= 132,			//���ṱ��
		TOOLTIP_FIELD_ITEM_EXPIRE_TIME				= 133,			//��ʱ���ߵ���ʱ
		TOOLTIP_FIELD_ITEM_BATCH_USE				= 134,			//����ʹ��

		TOOLTIP_FIELD_ITEM_BASE_SCORE				= 135,			//����ս����

		TOOLTIP_FIELD_SKILLBOOK_SCORE				= 136,			//�ؼ�ս����
		TOOLTIP_FIELD_SKILLBOOK_ATTRIB				= 137,			//�ؼ�����
		TOOLTIP_FIELD_SKILLBOOK_DES					= 138,			//�ؼ�����
		TOOLTIP_FIELD_AVATAR						= 139,			//avatar
		TOOLTIP_FIELD_TRANSFORMCARD_DES				= 140,			//�ؼ�����

		TOOLTIP_FIELD_ADDITEM_LINKINFO				= 141,			//����link��Ϣ
		TOOLTIP_FIELD_ITEM_EQUIP_SUPER_ATTRI		= 142,
		TOOLTIP_FIELD_ITEM_EQUIP_TOTEM_ATTRI		= 143,

		TOOLTIP_FIELD_ITEM_EQUIP_GEM_BUFF			= 144,			//��ʯ�ӳ�
		TOOLTIP_FIELD_ITEM_EQUIP_SLOT_ICON			= 145,			//װ����Ƕͼ��

		TOOLTIP_FIELD_SKILLBOOK_SKILLINFO			= 146,			//�ؼ���������
		TOOLTIP_FIELD_SKILLBOOK_TALENTINFO			= 147,			//�ؼ��츳����
		TOOLTIP_FIELD_SKILLBOOK_STARINFO			= 148,			//�ؼ���ʼ�Ǽ�����

	} TooltipFieldType;

	//����
	typedef enum
	{
		FONT_SMALL			= 0,							//12����
		FONT_MIDDLE			= 1,							//15����
		FONT_LARGE			= 3,							//20����
	} TooltipFieldFont;
	//�����С
	typedef enum
	{
		FONTSIZE_XSMALL		= 0,
		FONTSIZE_SMALL		= 1,							//С��
		FONTSIZE_MIDDLE		= 2,							//�к�	
		FONTSIZE_LARGE		= 3,							//���
	} TooltipFieldFontSize;
	//������ɫ
	typedef enum
	{
		//WHITE		= 0xFFFFFFFF,							//��ɫ
		//BLUE		= 0xFF0000FF,							//��ɫ
		//GRAY		= 0xFFC0C0C0,							//��ɫ
		//GOLDENROD	= 0xFFDAA520,							//����
		//RED			= 0xFFFF0000,							//��ɫ
		//GOLD		= 0xFFFFD700,							//��ɫ
		//GREEN		= 0xFF00FF00,							//��ɫ

		//���������������ɫ�ض����
		WHITE		= 0xffffffff,							//��ɫ
		WHITE2		= 0xffcecece,							//��ɫ
		YELLOW		= 0xffffea3c,							//��ɫ
		PURPLE		= 0xffb214e6,							//��ɫ
		BLUE		= 0xff02c4ff,							//��ɫ
		GRAY		= 0xff7d7d7d,							//��ɫ
		GRAY2		= 0xff858585,							//��ɫ2
		DARKGRAY	= 0xff6e6d6d,							//���ɫ
		GOLDENROD	= 0xffbe8247,							//����
		RED			= 0xffff0000,							//��ɫ
		GOLD		= 0xffe2b54a,							//��ɫ
		GOLD2		= 0xffc7b377,							//��ɫ2
		GREEN		= 0xff00FF00,							//��ɫ
		PINK		= 0xc4a2a0,								//��ɫ
		ORANGE		= 0xdbbd9b,								//��ɫ
		DARKORANGE	= 0xcd9a65,								//����ɫ
		NEWGOLD		= 0xffbe8247,
		BROWN		= 0xffbc863f,							//��ɫ
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

		//MEMBER_UNSHORT(Type);							//�ֶ�����
		MEMBER_STRING(Caption);							//
		MEMBER_BOOL(Enable);							//�Ƿ���ʾ���ֶ�
		MEMBER_STRING(Content);							//�ֶ�����
		MEMBER_UNSHORT(Font);							//����
		MEMBER_UNSHORT(FontSize);						//����ߴ�
		MEMBER_INT(FontColor);							//������ɫ
		MEMBER_UNSHORT(LengthLimit);					//�ֶγ�������
		MEMBER_UNSHORT(EntryNumLimit);					//�ֶ���Ŀ����
		MEMBER_BOOL(NewLine);							//�Ƿ���
	protected:
		//union
		//{
		//	unsigned short m_usLengthLimit;				//�ֶ����ݳ�������
		//	unsigned short m_usEntryNumLimit;			//�ֶ���Ŀ����
		//};
		
		TooltipFieldType m_kType;						//�ֶ�����

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

		//Begin ����tooltip�Ĳ����ӿ�
		void		  SetObserver(LC_PlayerBase* pkObserver){m_pkObserver = pkObserver;};
		LC_PlayerBase* GetObserver(){return m_pkObserver;};
		//End

		bool GetSkillStateUseResult(int lRecordID, StringType &kUseResult);

	protected:
		//Begin �ַ�������
		StringType _numArabToChinese(int lArabNum);					//������������ת���ɺ���
		bool	   _formatTooltip(TooltipFieldVector& kFieldVector);	//��ʽ��Tooltip����ʾ��ʽ����UI������ǰ��ʱʹ�øù��ܣ�

		int			_variable2Value(const StringType& kMemberName, int lRecordID);
		//End
		
		//�����ı���ȡ��Ʒ��ʹ��Ч��
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

			//����ungreedy��ʽ����ƥ��
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
				kContent = kPostfix;									//�������Һ����[]


				int iParamStart = 0;
				TStringVector kParamVector;
				TStringVector kOperatorVector;
				//��ȡ���в�����������ֺ������
				while ( kPattern.match(TPSTR2STDSTR(CodeConvert::WcsToUtf8(kParams))) > 0 )
				{
					PME::markers kParam1Pos = kPattern.GetMatchPos(1);
					PME::markers kParam2Pos = kPattern.GetMatchPos(2);

					int iOperatorStart = kParam1Pos.second;
					int iOperatorEnd = kParam2Pos.first;
					StringType kOperator = CodeConvert::WcsToUtf8(kParams.substr(iOperatorStart, iOperatorEnd - iOperatorStart));
					StringType kParam1 = CodeConvert::WcsToUtf8(kParams.substr(iParamStart, iOperatorStart - iParamStart));
					
					iParamStart = iOperatorEnd;
					
					kParamVector.push_back(kParam1);					//����������
					UT_BaseStringUtility::Trim(kOperator);
					kOperatorVector.push_back( kOperator );		//���������
				}
				//�������һ��������
				StringType kLastParam = CodeConvert::WcsToUtf8(kParams.substr(iParamStart, kParams.length() - iParamStart));
				kParamVector.push_back(kLastParam);


				//������������
				int iParamCount = kParamVector.size();
				int iOperatorCount = kOperatorVector.size();
				double fParamResult = 0;
				//���ֻ��һ����������û�������
				if (iParamCount == 1 && iOperatorCount <= 0)
				{
					const StringType & kParam = kParamVector[0];
					//���������Ǳ���������csv����Ѱ�Ҷ�Ӧ����ֵ
					if (atof(kParam.c_str()) - 0 < FLT_EPSILON)
					{
						const StringType & kMemberName( kParam );
						StringType kMemberType = "";
						StringType kMemberData = "";
						fParamResult = pkRecordList->DecodeNumMember(lRecordID, kMemberName, kMemberType, kMemberData);
					}
					else	//������������ֵ
					{
						fParamResult = atof(kParam.c_str());
					}	
				}
				else if (iOperatorCount >= 1)	//������������
				{
					if (iParamCount > 0)
					{
						//ȡ��һ��������
						const StringType & kParam = kParamVector[0];
						//���������Ǳ���������csv����Ѱ�Ҷ�Ӧ����ֵ
						if (atof(kParam.c_str()) - 0 < FLT_EPSILON)
						{
							const StringType & kMemberName(kParam);
							StringType kMemberType;
							StringType kMemberData;
							fParamResult = pkRecordList->DecodeNumMember(lRecordID, kMemberName, kMemberType, kMemberData);
							if(-1 == fParamResult)
							{
								//���＼��״̬���⴦��
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
						else	//������������ֵ
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
					//���������Ǳ���������csv����Ѱ�Ҷ�Ӧ����ֵ
					if (atof(kParam.c_str()) - 0 < FLT_EPSILON)
					{
						const StringType & kMemberName( kParam );
						StringType kMemberType;
						StringType kMemberData;
						fParam = pkRecordList->DecodeNumMember(lRecordID, kMemberName, kMemberType, kMemberData);
						if(-1 == fParam)
						{
							//���＼��״̬���⴦��
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
					else	//������������ֵ
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
				
				kStringStream << kPrefix << "<color val=0xff22ce4e>";					//��ֵǰ�����ֲ���
				long ts = Utility::Double2Long(fParamResult);
				if ( fParamResult - ts >= 0.1f)
				{
					kStringStream << fixed << setprecision(1) << fParamResult << "</color>" ;		//��ֵ���֣�ȡ������
				}
				else
				{
					kStringStream << ts << "</color>";		//��ֵ���֣�ȡ������
				}
			}

			kStringStream << kPostfix;
			kUseResult = CodeConvert::WcsToUtf8(kStringStream.str());

			return true;
		}

		
	private:
		TooltipFieldVector m_kTooltipFieldVector;		//����tooltip����
		LC_PlayerBase*	   m_pkObserver;				//tooltip�۲���
	};
}

#endif

