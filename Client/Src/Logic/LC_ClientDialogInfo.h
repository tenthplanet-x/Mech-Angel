#ifndef _LC_CLIENT_DIALOG_INFO_H
#define _LC_CLIENT_DIALOG_INFO_H

#define  MAX_DAILOG_SELECT_ITEM_COUNT	5
#define  MAX_DAILOG_NODE_COUNT			20
class LC_DialogNodeData
{
public:
	enum DialogEvent
	{
		DIALOG_EVENT_NONE					= 0x0000,
		DIALOG_EVENT_CLOSE_DIALOG			= 0x0001,
		DIALOG_EVENT_SHOW_TASK_FINISH_VIEW	= 0x0002,
		DIALOG_EVENT_SHOW_TASK_ACCEPT_VIEW	= 0x0004,
		DIALOG_EVENT_NOTICE_TASK_EVENT		= 0x0008,
		DIALOG_EVENT_TASK_CONDITION1_EVENT	= 0x0010,
		DIALOG_EVENT_TASK_CONDITION2_EVENT	= 0x0020,
		DIALOG_EVENT_TASK_CONDITION3_EVENT	= 0x0040,
	};

	enum DialogShowLimit
	{
		DIALOG_SHOWLIMIT_ALWAYS				= 0x0000,
		DIALOG_SHOWLIMIT_NEVER				= 0x0001,		
		DIALOG_SHOWLIMIT_CAN_ACCEPT_TASK	= 0x0002,
		DIALOG_SHOWLIMIT_PROCESSING_TASK	= 0x0004,
		DIALOG_SHOWLIMIT_CAN_FINISH_TASK	= 0x0008,
		DIALOG_SHOWLIMIT_FINISH_TASK		= 0x0010,
	};

public:
	LC_DialogNodeData();
	~LC_DialogNodeData();
	DECL_RTTI_NAME( LC_DialogNodeData )

	void		Reset();

	int			GetDialogEvent() {return m_iDialogEvent;};
	void		SetDialogEvent(int iDialogEvent){m_iDialogEvent = iDialogEvent;};

	int			GetDialogEventParam() {return m_iDialogEventParam;};
	void		SetDialogEventParam(int iDialogEventParam){m_iDialogEventParam = iDialogEventParam;};

	int			GetDialogShowLimit() {return m_iDialogShowLimit;};
	void		SetDialogShowLimit(int iShowLimit){m_iDialogShowLimit = iShowLimit;};

	const StringType &  GetDialogTitle() { return m_strDialogTitle;};
	void				SetDialogTitle(const StringType &strDialogTitle){m_strDialogTitle = strDialogTitle;};

	const StringType &  GetDialogContent(){return m_strDialogContent;};
	void				SetDialogContent(const StringType &strDialogTitle){m_strDialogContent = strDialogTitle;};

	int			GetDialogSelectIndex(int index);
	void		SetDialogSelectIndex(int index, int iSelectIndex);

protected:
	int			m_iDialogEvent;
	int			m_iDialogEventParam;
	int			m_iDialogShowLimit;
	StringType	m_strDialogTitle;
	StringType	m_strDialogContent;
	int			m_iDialogSelectIndex[MAX_DAILOG_SELECT_ITEM_COUNT];
};

class LC_DialogSet	: public Memory::MM_BaseObject
{
public:
	enum DialogSetType
	{
		DST_NORMAL = 0,
		DST_TASK   = 1,
	};
public:
	LC_DialogSet();
	~LC_DialogSet();
	DECL_RTTI_NAME( LC_DialogSet )

	LC_DialogNodeData* GetDialogNodeData(int iIndex);
	void			   AddDialogNodeData(LC_DialogNodeData& rDialogNode);
	int				   GetDialogNodeCount();

	int				   GetParamType(){return m_iParamType;};
	void			   SetParamType(int iParamType){ m_iParamType = iParamType;};

	long     		   GetParam(){return m_lParam;};
	void			   SetParam(long iParam){m_lParam = iParam;};

protected:
	typedef mem::vector<LC_DialogNodeData>  LC_DialogNodeList;
	LC_DialogNodeList	m_kDialogList;

	int			m_iParamType;
	long		m_lParam;
};

class LC_ClientDialogInfo : public Memory::MM_BaseObject
{
public:
	LC_ClientDialogInfo();
	~LC_ClientDialogInfo();
	DECL_RTTI_NAME( LC_ClientDialogInfo )

	int					GetDialogSetCount();
	LC_DialogSet*		AllocateDialogSet();
	void				AddDialogSet(LC_DialogSet* pkDialogSet);
	LC_DialogSet*		GetDialogSet(int iIndex);
protected:
		typedef mem::list<LC_DialogSet*>		CharDialogSetList;
	typedef CharDialogSetList::iterator		CharDialogSetListIter;				

	CharDialogSetList	m_kDialogInfo;

};
#endif