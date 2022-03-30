#ifndef _CLIENT_YUNYING_PLATFORM_H
#define _CLIENT_YUNYING_PLATFORM_H 


/*************************************************************************
    �Խ�ƽ̨���
*************************************************************************/
//��ʼ����Ѷ��ƽ̨���Lua��������Lua��ջ��ʼ��֮�����
void InitPlatformLuaFunc();
void UnInitPlatformLuaFunc();
//
//////////////////////////////////////////////////////////////////////////


YunYingPlatform		getYunYingplatform();
YunYingPlatform		getYunYingplatform(const StringType & platform);
PlatformPage_Type	getYunYingPlatformPage();

//////////////////////////////////////////////////////////////////////////

//< tx�Խӣ���ƽ̨

enum	TxSubPlatform
{
	//< notice�������ֵһ��д�ã���������ģ��漰�����
	TSP_NA = -1,
	TSP_QZONE	= 0,	//< �ռ�			����
	TSP_WEBSITE = 1,	//< x5΢��				����
	TSP_QQGAME  = 2,	//< ����				����
	TSP_PENGYOU = 3,	//< pengyou			����
	TSP_QPLUS	= 4,	//< x5qplus				����
	TSP_3366	= 5,	//< 3366				����
	TSP_TGP		= 6,	//< 

	TSP_UNION_BEGIN		= 7,
	TSP_UNION_IWAN		= 8,	//- union-10029			����
	TSP_UNION_TGP		= 9,	// union-10153-$id		union-10055-$id	���ꡢ
	TSP_UNION_BROWSER	= 10,	//-- union-10194-$id		����
	TSP_UNION_GJ_BOX	= 11,	//-- union-10210-$id		����
	TSP_UNION_3366		= 12,	//-- union-10131			3366����2
	TSP_UNION_PET		= 13,	//-- union-10215-1		QQ����
	TSP_UNION_DEFAULT	= 29,
	TSP_UNION_END		= 30,
};

enum	TxVip
{
	TSV_NA = -1,
	TSV_YELLOW,		//< ����
	TSV_BLUE,		//< ����
};

enum	YunYingOpenUrlType
{
	YYOUT_DEFAULT = 0,
	YYOUT_CURRENT = 1,
	YYOUT_IEXPLORE= 2,
};
StringType			getPlatformVipTypeImage(int iLevel, bool bHigh);
StringType			getPlatformVipTypeYearImage();
TxSubPlatform		getTxSubplatform();
TxSubPlatform		getTxSubplatform(const StringType& platform);
TxVip				getTxSubplatformVip();
bool				isTxUnion();

//////////////////////////////////////////////////////////////////////////

bool	   showWebWindow(const char * url, int web_width, int web_height);
bool	   showInnerWebWindow(const char * url, int web_width, int web_height, int x, int y);
void	   resetInnerWebWindow(int width, int height, int x, int y);
void	   hideWebWindow();

bool	   showShunWangWebWindow(const char * url, const char* jump_url, int web_width, int web_height);

//////////////////////////////////////////////////////////////////////////
//< ���ýӿ�
struct YunYingReportArg
{
	static YunYingReportArg & singleton();

	/////////////////////////////////
	bool		d_newrole;						//< ���û�(ͨ��)
	int			d_newroleRegTime;				//< ע��ʱ��(ͨ��)

	bool		d_duowanChatReportable;			//< duowan�����ϱ�

	bool		d_game2NewRole3minReported;		//< game2���û�����3�����Ƿ����ϱ�
	DWORD		d_game2NewRole3minBeginTime;	//< game2���û�����3���Ӽ�ʱ��

	DWORD		d_txLastKeepAliveTime;			//< tx�ϴ�����ʱ��

	/////////////////////////////////
	void		setnewrole(bool b);
	void		setnewrole_regtime(int t);
	void		setduowan_reportable(bool b);
	void		setgame2newrole3min_reported(bool b);
	void		setgame2newrole3min_begintime(DWORD t);
	void		settx_keepallivetime(DWORD t);
};

//< ���ýӿ�


//< tx�Խӣ����̽ӿ�
bool		report2tx(const char *acttype, const char * postfix);
void		report2tx_login(object_id_type uid, int level);
void		report2tx_quit(object_id_type uid, int level, int onlinetime);
void		report2tx_register(object_id_type uid);	//< register/accept
void		report2tx_consume(object_id_type uid, MoneyType yuanbao);
//< tx�Խӣ�����
bool		report2tx_keepalive();

//< 52xiyou�Խӣ����̽ӿ�
bool		report2xy(const char *acttype, const char * postfix);
void		report2xy_login(const StringType & role_name, int role_level, int role_profession);

//< 360�Խ�
//< ��ɫ��Ϣ�ϱ�
void		report2360wan_levelup(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power);
void		report2360youxi_levelup(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power);

//< 360�Խ�

//< duowan�Խ�
//< ��ɫ��Ϣ�ϱ�
void		report2duowan_getreportable(int sceneId, const StringType & sid, const StringType & rolename);
void		report2duowan_register(unique_id_type uuid, const char * role_name, int role_gender, int role_level, int role_profession);
void		report2duowan_levelup(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power);
void		report2duowan_chatmsg(const StringType & chatContent, const StringType & toUserName, const StringType & fromUserName, int bMultipy, int sceneId);

//< �첽�ӿ�
void		report2duowan_async_getreportable(int sceneId, const char * rolename);
void		report2duowan_async_register(const char * role_name, int role_gender, int role_level, int role_profession);
void		report2duowan_async_levelup(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power);
void		report2duowan_async_chatmsg(const char * chatContent, const char * toUserName, const char * fromUserName, int bMultipy, int sceneId);



#define		DUOWAN_GKEY		L"JZWL"
//< yy�Խ�


#endif
