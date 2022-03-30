#ifndef _CLIENT_YUNYING_PLATFORM_H
#define _CLIENT_YUNYING_PLATFORM_H 


/*************************************************************************
    对接平台相关
*************************************************************************/
//初始化腾讯子平台相关Lua函数，在Lua堆栈初始化之后调用
void InitPlatformLuaFunc();
void UnInitPlatformLuaFunc();
//
//////////////////////////////////////////////////////////////////////////


YunYingPlatform		getYunYingplatform();
YunYingPlatform		getYunYingplatform(const StringType & platform);
PlatformPage_Type	getYunYingPlatformPage();

//////////////////////////////////////////////////////////////////////////

//< tx对接，子平台

enum	TxSubPlatform
{
	//< notice：这里的值一旦写好，不允许更改，涉及到配表
	TSP_NA = -1,
	TSP_QZONE	= 0,	//< 空间			黄钻
	TSP_WEBSITE = 1,	//< x5微端				蓝钻
	TSP_QQGAME  = 2,	//< 大厅				蓝钻
	TSP_PENGYOU = 3,	//< pengyou			黄钻
	TSP_QPLUS	= 4,	//< x5qplus				蓝钻
	TSP_3366	= 5,	//< 3366				蓝钻
	TSP_TGP		= 6,	//< 

	TSP_UNION_BEGIN		= 7,
	TSP_UNION_IWAN		= 8,	//- union-10029			蓝钻
	TSP_UNION_TGP		= 9,	// union-10153-$id		union-10055-$id	蓝钻、
	TSP_UNION_BROWSER	= 10,	//-- union-10194-$id		蓝钻
	TSP_UNION_GJ_BOX	= 11,	//-- union-10210-$id		蓝钻
	TSP_UNION_3366		= 12,	//-- union-10131			3366官网2
	TSP_UNION_PET		= 13,	//-- union-10215-1		QQ宠物
	TSP_UNION_DEFAULT	= 29,
	TSP_UNION_END		= 30,
};

enum	TxVip
{
	TSV_NA = -1,
	TSV_YELLOW,		//< 黄钻
	TSV_BLUE,		//< 蓝钻
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
//< 公用接口
struct YunYingReportArg
{
	static YunYingReportArg & singleton();

	/////////////////////////////////
	bool		d_newrole;						//< 新用户(通用)
	int			d_newroleRegTime;				//< 注册时间(通用)

	bool		d_duowanChatReportable;			//< duowan聊天上报

	bool		d_game2NewRole3minReported;		//< game2新用户在线3分钟是否已上报
	DWORD		d_game2NewRole3minBeginTime;	//< game2新用户在线3分钟计时器

	DWORD		d_txLastKeepAliveTime;			//< tx上次续命时间

	/////////////////////////////////
	void		setnewrole(bool b);
	void		setnewrole_regtime(int t);
	void		setduowan_reportable(bool b);
	void		setgame2newrole3min_reported(bool b);
	void		setgame2newrole3min_begintime(DWORD t);
	void		settx_keepallivetime(DWORD t);
};

//< 公用接口


//< tx对接，罗盘接口
bool		report2tx(const char *acttype, const char * postfix);
void		report2tx_login(object_id_type uid, int level);
void		report2tx_quit(object_id_type uid, int level, int onlinetime);
void		report2tx_register(object_id_type uid);	//< register/accept
void		report2tx_consume(object_id_type uid, MoneyType yuanbao);
//< tx对接，续命
bool		report2tx_keepalive();

//< 52xiyou对接，罗盘接口
bool		report2xy(const char *acttype, const char * postfix);
void		report2xy_login(const StringType & role_name, int role_level, int role_profession);

//< 360对接
//< 角色信息上报
void		report2360wan_levelup(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power);
void		report2360youxi_levelup(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power);

//< 360对接

//< duowan对接
//< 角色信息上报
void		report2duowan_getreportable(int sceneId, const StringType & sid, const StringType & rolename);
void		report2duowan_register(unique_id_type uuid, const char * role_name, int role_gender, int role_level, int role_profession);
void		report2duowan_levelup(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power);
void		report2duowan_chatmsg(const StringType & chatContent, const StringType & toUserName, const StringType & fromUserName, int bMultipy, int sceneId);

//< 异步接口
void		report2duowan_async_getreportable(int sceneId, const char * rolename);
void		report2duowan_async_register(const char * role_name, int role_gender, int role_level, int role_profession);
void		report2duowan_async_levelup(int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power);
void		report2duowan_async_chatmsg(const char * chatContent, const char * toUserName, const char * fromUserName, int bMultipy, int sceneId);



#define		DUOWAN_GKEY		L"JZWL"
//< yy对接


#endif
