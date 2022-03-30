
//������Ϸ�����е�������ʾ��������Ϣ
#ifndef TT_TEXT_MANAGER_H
#define TT_TEXT_MANAGER_H
#include "LM_LocaleManager.h"
#include "MG_MessageHandlerInterface.h"
#include "ClientDefine.h"

#include "CF_TextConfig.h"

#define ALL_MAX_MSG_NUM			3000
#define SUB_MAX_MSG_NUM			500
#define MAX_TRUMPET_NUM			200


#define TEXT_PARAM_NUM			6
#define TEXT_WILD_CHAR_NUM		5
#define TEXT_UINT64_NUM			2

#define TEXT_CHANNEL_TIP_TIME				5.0f
#define TEXT_CHANNEL_TRANSCRIPTION_TIME		15.0f
#define TEXT_CHANNEL_SUBCHAT_TIME			10.0f
#define TEXT_CHANNEL_ACTIVITY_TIME			5.0f
#define TEXT_CHANNEL_RAGEUP_TIME			3.0f
#define TEXT_CHANNEL_TREASURE_TIME			3.0f
#define SUMMOR_TALK_SUFFIX		GET_MSG(TT_TEXTMANAGER_H_001)


//Ƶ������
enum TextChannelType
{
	TEXT_CHANNEL_TYPE_ANNOUNCE					=		0x00001,				//����������ϵͳ��ʾ
	TEXT_CHANNEL_TYPE_TIP						=		0x00002,				//�������Ϸ���������
	TEXT_CHANNEL_TYPE_CHAT						=		0x00004,				//����Ƶ��
	TEXT_CHANNEL_TYPE_TRUMPET					=		0x00008,				//С����Ƶ��
	TEXT_CHANNEL_TYPE_FRIEND					=		0x00010,				//����˽��Ƶ��
	TEXT_CHANNEL_TYPE_MESSAGE_PROMPT			=		0x00020,				//��ģʽ�Ի���
	TEXT_CHANNEL_TYPE_MESSAGE_BOX				=		0x00040,				//ֻ��ȷ����ť�ĶԻ���
	TEXT_CHANNEL_TYPE_MESSAGE_DIALOG			=		0x00080,				//��ȷ����ȡ���ĶԻ���
	TEXT_CHANNEL_TYPE_BUBBLE_TALKING			=		0x00100,				//�Ի�����Ƶ��
	TEXT_CHANNEL_TYPE_WORLD_NAME				=		0x00200,				//����������ʾ
	TEXT_CHANNEL_TYPE_TRANSCRIPTION				=		0x00400,				//������ʾ��Ϣ
	TEXT_CHANNEL_TYPE_SUPER_TRUMPET				=		0x00800,				//������Ƶ��
	TEXT_CHANNEL_TYPE_ACTIVITY					=		0x01000,				//��Ļ�·���������
	TEXT_CHANNEL_TYPE_MOUSE						=		0x02000,				//���������ʾ��Ϣ
	TEXT_CHANNEL_TYPE_SUBCHAT					=		0x04000,				//�������
	TEXT_CHANNEL_TYPE_WIDGET					=		0x08000,				//���洰����ʾ��Ϣ
	TEXT_CHANNEL_TYPE_RAGEUP					=		0x10000,				//ŭ��ֵ�Ϸ���ʾ
	TEXT_CHANNEL_TYPE_TREASURE					=		0x20000,				//���ұ���ȫ��֪ͨ
};

//Ƶ����ҳ����
enum ChannelTabType
{
	GENERAL_CHANNEL_TAB_TYPE_NORMAL				=		0,				//ͨ�ÿؼ�����ҳ

	CHAT_CHANNEL_TAB_TYPE_NORMAL				=		1,				//�����ۺ���ҳ
	CHAT_CHANNEL_TAB_TYPE_SYSTEM				=		2,				//����ϵͳ��ҳ
	CHAT_CHANNEL_TAB_TYPE_FIGHT					=		3,				//����ս����ҳ
	CHAT_CHANNEL_TAB_TYPE_PRIVATE				=		4,				//���������ҳ
	CHAT_CHANNEL_TAB_TYPE_CUSTOM				=		5,				//�����Զ�����ҳ
	CHAT_CHANNEL_TAB_TYPE_AUXILIARY				=		6,				//���츨����ҳ
	CHAT_CHANNEL_TAB_TYPE_NEARBY				=		7,				//���츽����ҳ
	CHAT_CHANNEL_TAB_TYPE_TEAM					=		8,				//���������ҳ
	CHAT_CHANNEL_TAB_TYPE_GUILD					=		9,				//���칫����ҳ
	CHAT_CHANNEL_TAB_TYPE_WORLD					=		10,			//����������ҳ
	CHAT_CHANNEL_TAB_TYPE_TRUMPET				=		11,			//����������ҳ
};

//��Ϣ�ṹ�壬UI����ֱ�ӷ���
class TT_TextElement
{
public:
	DECL_RTTI_NAME(TT_TextElement)

	StringType			m_strText;

	//��Ϣ����
	long				m_lTextMessageType;
	//��ʱ�������<0Ϊ������Ч
	long				m_lTextLifeTime;

	//�յ�ϵͳʱ��
	StringType			m_strServerTime;

	//��������
	StringType			m_strBaseText;
	//�ַ���ͨ����б�
	StringType			m_strWildCards[TEXT_WILD_CHAR_NUM];
	//��ʽ���õ��ַ���ͨ����б�
	StringType			m_strFormatWildCards[TEXT_WILD_CHAR_NUM];
	//���β����б�
	long				m_lParameters[TEXT_PARAM_NUM];
	//64λ�����б�
	uint64_t				m_ui64Parameters[TEXT_UINT64_NUM];
	//��ʾ��Ϣ�ȼ�
	int					m_iLevel;

	TT_TextElement()
	{
		Reset();
	}

	TT_TextElement(const TT_TextElement& rkTextElement)
	{
		m_strText			= rkTextElement.m_strText;
		m_lTextMessageType	= rkTextElement.m_lTextMessageType;
		m_lTextLifeTime		= rkTextElement.m_lTextLifeTime;
		m_strServerTime		= rkTextElement.m_strServerTime;
		m_strBaseText		= rkTextElement.m_strBaseText;
		m_iLevel			= rkTextElement.m_iLevel;

		for(int i = 0; i < TEXT_WILD_CHAR_NUM; i++)
			m_strWildCards[i] = rkTextElement.m_strWildCards[i];

		for(int i = 0; i < TEXT_WILD_CHAR_NUM; i++)
			m_strFormatWildCards[i] = rkTextElement.m_strFormatWildCards[i];
		
		memcpy(m_lParameters,rkTextElement.m_lParameters,sizeof(long) * TEXT_PARAM_NUM);
		memcpy(m_ui64Parameters,rkTextElement.m_ui64Parameters,sizeof(uint64_t) * TEXT_UINT64_NUM);
	}

	TT_TextElement& operator =(const TT_TextElement& rkTextElement)
	{
		if (&rkTextElement == this)
		{
			return *this;
		}
		m_strText			= rkTextElement.m_strText;
		m_lTextMessageType	= rkTextElement.m_lTextMessageType;
		m_lTextLifeTime		= rkTextElement.m_lTextLifeTime;
		m_strServerTime		= rkTextElement.m_strServerTime;
		m_strBaseText		= rkTextElement.m_strBaseText;
		m_iLevel			= rkTextElement.m_iLevel;

		for(int i = 0; i < TEXT_WILD_CHAR_NUM; i++)
			m_strWildCards[i] = rkTextElement.m_strWildCards[i];

		for(int i = 0; i < TEXT_WILD_CHAR_NUM; i++)
			m_strFormatWildCards[i] = rkTextElement.m_strFormatWildCards[i];

		memcpy(m_lParameters,rkTextElement.m_lParameters,sizeof(long) * TEXT_PARAM_NUM);
		memcpy(m_ui64Parameters,rkTextElement.m_ui64Parameters,sizeof(uint64_t) * TEXT_UINT64_NUM);
		return *this;
	}


	void Reset()
	{
		m_lTextMessageType	= 0;
		m_lTextLifeTime		= 0;
		m_strText			= "";
		memset(m_lParameters,0,sizeof(long) * TEXT_PARAM_NUM);
		memset(m_ui64Parameters,0,sizeof(uint64_t) * TEXT_UINT64_NUM);
		for(int i = 0; i < TEXT_WILD_CHAR_NUM; i++)
			m_strWildCards[i] = "";
		m_strServerTime		= "";
		m_iLevel = 0;
	}

	const StringType&	GetText(){return m_strText;};
	const StringType&	GetBaseText(){return m_strBaseText;};
	long				GetTextLifeTime(){return m_lTextLifeTime;};
	long				GetTextMessageType(){return m_lTextMessageType;};
	const StringType&   GetServerTime(){return m_strServerTime;};
	int					GetLevel(void)const{return m_iLevel;}


	const long GetLongParameter(int i)
	{
		if(i < 0 || i >= TEXT_PARAM_NUM)
			return 0;

		return m_lParameters[i];
	}

	const uint64_t GetInt64Parameter(int i)
	{
		if(i < 0 || i >= TEXT_UINT64_NUM)
			return 0;

		return m_ui64Parameters[i];
	}
	StringType GetWildCards(int i)
	{
		if(i < 0 || i >= TEXT_WILD_CHAR_NUM)
			return "";

		return m_strWildCards[i];
	}

	StringType GetFormatWildCard(int i)
	{
		if(i < 0 || i >= TEXT_WILD_CHAR_NUM)
			return "";

		return m_strFormatWildCards[i];
	}
};

typedef mem::deque<TT_TextElement>		TT_TextElementList;
typedef TT_TextElementList::iterator	TT_TextElementListIter;


//��Ϣ����������ÿ��Ƶ��һ��
class TT_TextElementContainer : public Memory::MM_BaseObject
{
public:
	TT_TextElementContainer();
	~TT_TextElementContainer();

	void						Reset();
	void						SetContainerMaxSize(int iSize){m_iContainerMaxSize = iSize;};
	int							AddTextElement(TT_TextElement& rkTextElement,int& iRemoveTextCount);
	TT_TextElement*				GetTextElement(int iIndex);
	int							GetTextElementCount();


	int							m_iContainerMaxSize;
	int							m_iTextElementIncreaseNum;
	TT_TextElementList			m_kTextElementList;
};

//��ҳ��Ƶ�����ݵ�����
typedef mem::deque<long>			TT_TextIndexList;
typedef TT_TextIndexList::iterator	TT_TextIndexListIter;


struct TT_ChannelTabData			: public Memory::MM_BaseObject
{
	TT_TextIndexList  m_kList;
	int				  iTabMaxTextCount;
	bool			  m_bOrderDirtyFlag;
};

//Ƶ�����ݹ��� 
class TT_ChannelTextDataManager		: public Memory::MM_BaseObject
{
public:
	TT_ChannelTextDataManager();
	~TT_ChannelTextDataManager();
	
	void			Reset();

	void			RegisterTab(ChannelTabType eChannelTabType,int iTabMaxTextCount);

	void			SetChannelMaxTextCount(int iCount);
	int				AddTextElement(TT_TextElement& rkTextElement,int& iRemoveTextCount);
	void			AddTextElementIndexToTab(int iTabType,int iIndex);
	void			AddTrumpetTextElementIndexToTab(int iTabType, int iIndex);
	void			RemoveTextElementIndexInTabs(int iIndex);
	void			Update(float fCurrentTime);

	int				GetTabTextCount(int iTabType);
	int				GetTextElementCount(){return m_kChannelTextDataContainer.GetTextElementCount();}
	int				GetAndClearTextIncreaseNum();
	bool			GetAndClearTabOderDirtyFlag(int iTabType);
	void			ClearTabText(int iTabType);


	TT_TextElement* GetTabTextElement(int iTabType,int iIndex);
	TT_TextElement* GetTextElement(int iTextIndex);
protected:
	typedef mem::map<int,TT_ChannelTabData*>		TT_ChannelTabMap;
	typedef TT_ChannelTabMap::iterator				TT_ChannelTabMapIter;


	TT_ChannelTabMap			m_kChannelTabMap;
	TT_TextElementContainer		m_kChannelTextDataContainer;
};

class TT_TextManager : public Utility::UT_Singleton<TT_TextManager>,
					   public MG_MessageHandlerInterface,
				       public Memory::MM_BaseObject
{
public:
	TT_TextManager();
	virtual ~TT_TextManager();
	DECL_RTTI_NAME( TT_TextManager )
	
	void				Init(void);
	void				UnInit(void);

	void				Reset();
	virtual bool		RecvMessage(MG_MessageBase& rkMessage);

	int					GetChannelTabTextCount(int iChannelType,int iTabType);
	int					GetChannelTextElementCount(int iChannelType);
	void				ClearChannelTabText(int iChannelType,int iTabType);
	bool				GetAndClearChannelTabOderDirtyFlag(int iChannelType,int iTabType);
	int					GetAndClearTextIncreaseNum(int iChannelType);
	TT_TextElement*     GetChannelTabTextElement(int iChannelType,int iTabType,int iIndex);
	TT_TextElement*		GetChannelTextElement(int iChannelType, int iTextIndex);

	void				AddTextElement(TT_TextElement& rkTextElement);

	static void			EncodeXMLString( const StringType& rkString, StringType& rOutString );
	
	void				SetAddToAuxiliaryTabFlag(bool bFlag){m_bAddToAuxiliaryTab = bFlag;};
	bool				IsAddToAuxiliaryTab(){return m_bAddToAuxiliaryTab;};

	StringType			GetServerTimeString();
	bool				IsInChannel(int iMessageType, int iChannelType, int iTabType);
protected:
	void			_event_TimerTick(MG_MessageBase& rkMessage);
	StringType		_getTextContent(TT_TextElement& rkTextElement,CSVFile::CF_TextConfig::DataEntry* pkConfigData,TextChannelType eChannelType);
	void			_fillColor(const TT_TextElement& rkTextElement, CSVFile::CF_TextConfig::DataEntry* pkConfigData,TextChannelType eChannelType, long &lBaseColor, long (&lParamColors)[5]);
	TT_ChannelTextDataManager     m_kChannel_Announce;
	TT_ChannelTextDataManager     m_kChannel_Tip;
	TT_ChannelTextDataManager     m_kChannel_Chat;
	TT_ChannelTextDataManager     m_kChannel_SubChat;
	TT_ChannelTextDataManager     m_kChannel_Trumpet;
	TT_ChannelTextDataManager     m_kChannel_SuperTrumpet;
	TT_ChannelTextDataManager     m_kChannel_Friend;
	TT_ChannelTextDataManager     m_kChannel_MessagePrompt;
	TT_ChannelTextDataManager     m_kChannel_MessageBox;
	TT_ChannelTextDataManager     m_kChannel_MessageDialog;
	TT_ChannelTextDataManager     m_kChannel_BubbleTalking;
	TT_ChannelTextDataManager     m_kChannel_WorldName;
	TT_ChannelTextDataManager     m_kChannel_Transcription;
	TT_ChannelTextDataManager	  m_kChannel_ActivityTip;
	TT_ChannelTextDataManager	  m_kChannel_MouseTip;
	TT_ChannelTextDataManager	  m_kChannel_WidgetTip;
	TT_ChannelTextDataManager	  m_kChannel_RageUP;
	TT_ChannelTextDataManager	  m_kChannel_Treasure;
	//�����촰�ڿ��ܻ������Ϣ����ɸѡ�߼���������Ӵ˱���������Ϣ�ض���
	bool						  m_bAddToAuxiliaryTab;

private:
	static int _logErrFunc(const char* sMsg);
};

#endif
