#include "ClientPCH.h"
#include "YunYingPlatform.h"
#include "CWebPage.h"
#include "UT_Curl.h"
#include "md5.h"
#include "LianYun/duowan/include/yggamemsg_loadlibrary.h"
#include "MG_MessageCenter.h"
#include "MG_MessageBase.h"
#include "CF_YunYingPlatformConfig.h"
#include <shellapi.h>

//----------------------------------------------------------------------------//
struct	PlatformInfo
{
	YunYingPlatform		d_platform;
	TxSubPlatform		d_subplatform;
	PlatformPage_Type	d_platformPage;
	PlatformInfo()
	{
		d_platform	  = YYP_SHUNWANG;
		d_subplatform = TSP_NA;
		d_platformPage = PTG_INIT;
	}
};

static PlatformInfo		gs_pfinfo;

static Utility::LC_LuaFunction<int>*			gs_getTxSubplatformVipFunc;
static Utility::LC_LuaFunction<StringType>*		gs_getPlatformVipTypeImageFunc;
static Utility::LC_LuaFunction<StringType>*		gs_getPlatformVipTypeYearImageFunc;
static Utility::LC_LuaFunction<bool>*			gs_isTxUnionFunc;

//////////////////////////////////////////////////////////////////////////
void InitPlatformLuaFunc()
{
	Utility::LS_LuaState* pkLuaState = Utility::LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	if (!pkLuaState->DoFile("Data\\Script\\UI\\YunYingPlatform.lua"))
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL ,LOG_SWITCH_CLIENT_CRITICAL ,
			"load ui scripts:%s failed!","Data\\Script\\UI\\YunYingPlatform.lua");
		T_ASSERT_CRITICAL(false,"InitTxSubPlatformFunc YunYingPlatform scripts error 0x00");
	}

	LS_LuaObject* pkLuaObject = NULL;
	pkLuaObject = pkLuaState->DirectGet("getTxSubplatformVip");
	T_ASSERT_CRITICAL_S(pkLuaObject && pkLuaObject->IsFunction());
	gs_getTxSubplatformVipFunc = T_NEW_D LC_LuaFunction<int>(pkLuaObject);

	pkLuaObject = pkLuaState->DirectGet("getPlatformVipTypeImage");
	T_ASSERT_CRITICAL_S(pkLuaObject && pkLuaObject->IsFunction());
	gs_getPlatformVipTypeImageFunc = T_NEW_D LC_LuaFunction<StringType>(pkLuaObject);

	pkLuaObject = pkLuaState->DirectGet("getPlatformVipTypeYearImage");
	T_ASSERT_CRITICAL_S(pkLuaObject && pkLuaObject->IsFunction());
	gs_getPlatformVipTypeYearImageFunc = T_NEW_D LC_LuaFunction<StringType>(pkLuaObject);


	pkLuaObject = pkLuaState->DirectGet("isTxUnion");
	T_ASSERT_CRITICAL_S(pkLuaObject && pkLuaObject->IsFunction());
	gs_isTxUnionFunc = T_NEW_D LC_LuaFunction<bool>(pkLuaObject);
}
void UnInitPlatformLuaFunc()
{
	T_SAFE_DELETE(gs_getTxSubplatformVipFunc);
	T_SAFE_DELETE(gs_getPlatformVipTypeImageFunc);
	T_SAFE_DELETE(gs_getPlatformVipTypeYearImageFunc);
	T_SAFE_DELETE(gs_isTxUnionFunc);
}
static CF_YunYingPlatformConfig::DataEntry* getPlatformConfigEntry(const StringType& platform)
{
	CF_YunYingPlatformConfig* insta = SafeGetSingleton<CF_YunYingPlatformConfig>();
	if (insta == NULL)
	{
		return NULL;
	}
	CF_YunYingPlatformConfig::DataEntryMapExternIterator i = insta->GetIterator();
	while (i.HasMoreElements())
	{
		CF_YunYingPlatformConfig::DataEntry* pentry = i.PeekNextValuePtr();
		if (pentry->_iFormatType == 0 && pentry->_Platform == platform)
		{
			return pentry;
		}
		else if (pentry->_iFormatType == 1 && UT_BaseStringUtility::startsWith(platform, pentry->_Platform, false))
		{
			return pentry;
		}
		else if (pentry->_iFormatType == 2 && UT_BaseStringUtility::contains(platform, pentry->_Platform, false))
		{
			return pentry;
		}
		i.MoveNext();
	}
	return NULL;
}
YunYingPlatform getYunYingplatform( )
{
	static	int	ls_initialized = 0;
	if (!ls_initialized)
	{
		ls_initialized = 1;

		StringType strPlatform;
		if (ClientCmdlineArg::TryGetValue(CMD_ARG_PLATFORM, strPlatform))
		{
			gs_pfinfo.d_platform = getYunYingplatform(strPlatform);
		}
	}
	return gs_pfinfo.d_platform;
}

YunYingPlatform getYunYingplatform( const StringType& platform )
{
	CF_YunYingPlatformConfig::DataEntry* pEntry = getPlatformConfigEntry(platform);
	return pEntry ? (YunYingPlatform)pEntry->_PlatformType : YYP_NA;
}

PlatformPage_Type getYunYingPlatformPage()
{
	static bool pfp_inited = false;
	if (!pfp_inited)
	{
		pfp_inited = true;

		YunYingPlatform platform = getYunYingplatform();

		if(platform == YYP_SHUNWANG)
		{
			StringType strPlatformPage;
			if (ClientCmdlineArg::TryGetValue(CMD_ARG_CLIENT_TYPE, strPlatformPage))
			{
				if(strPlatformPage == "pc")
					gs_pfinfo.d_platformPage = PTG_ShunWangWeiMicro;
				else if(strPlatformPage == "box")
					gs_pfinfo.d_platformPage = PTG_ShunWangBox;
				else
					gs_pfinfo.d_platformPage = PTG_ShunWangWeb;
			}
		}
	}
	
	return gs_pfinfo.d_platformPage;
}

TxSubPlatform getTxSubplatform()
{
	static	int	ls_initialized = 0;
	if (!ls_initialized)
	{
		ls_initialized = 1;

		StringType strPlatform;
		if (ClientCmdlineArg::TryGetValue(CMD_ARG_PLATFORM_LOGIN, strPlatform))
		{
			gs_pfinfo.d_subplatform = getTxSubplatform(strPlatform);
		}
	}
	return gs_pfinfo.d_subplatform;
}

TxSubPlatform getTxSubplatform(const StringType& platform)
{
	CF_YunYingPlatformConfig::DataEntry* pEntry = getPlatformConfigEntry(platform);
	return pEntry ? (TxSubPlatform)pEntry->_SubPlatformType : TSP_NA;
}


TxVip getTxSubplatformVip()
{
	TxSubPlatform tsp = getTxSubplatform();

	return (TxVip)(*gs_getTxSubplatformVipFunc)(tsp);
}

bool isTxUnion()
{
	TxSubPlatform tsp = getTxSubplatform();

	return (*gs_isTxUnionFunc)(tsp);
}

StringType getPlatformVipTypeImage(int iLevel, bool bHigh)
{
	return (*gs_getPlatformVipTypeImageFunc)(iLevel, bHigh);
}

StringType getPlatformVipTypeYearImage()
{
	return (*gs_getPlatformVipTypeYearImageFunc)();
}

/************************************************************************/

YunYingReportArg & YunYingReportArg::singleton()
{
	static YunYingReportArg ls_singleton;
	return ls_singleton;
}

void YunYingReportArg::setnewrole( bool b )
{
	d_newrole = b;
}

void YunYingReportArg::setnewrole_regtime( int t )
{
	d_newroleRegTime = t;
}

void YunYingReportArg::setduowan_reportable( bool b )
{
	d_duowanChatReportable = b;
}

void YunYingReportArg::setgame2newrole3min_reported( bool b )
{
	d_game2NewRole3minReported = b;
}

void YunYingReportArg::setgame2newrole3min_begintime( DWORD t )
{
	d_game2NewRole3minBeginTime = t;
}

void YunYingReportArg::settx_keepallivetime( DWORD t )
{
	d_txLastKeepAliveTime = t;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

static HANDLE  s_hExternalWebProcess = NULL;	// 外部支付进程句柄
static HWND	   s_hInternalWebWnd = 0;			// 内部支付页面窗口句柄
static DWORD   s_nWebOpenTime = 0;				// 页面打开时间

//////////////////////////////////////////////////////////////////////////
// 外部支付进程，避免造成游戏卡顿

static void hide_external_web_window(bool bTerminateProcess)
{
	if (s_hExternalWebProcess != NULL)
	{
		if (bTerminateProcess)
			::TerminateProcess(s_hExternalWebProcess, 0);
		::CloseHandle(s_hExternalWebProcess);
		s_hExternalWebProcess = NULL;
	}
}

static int show_external_web_window( const char * url, const char* jump_url, int web_width, int web_height )
{
	HINSTANCE	hInstance  = Utility::g_hWindowsInst;
	HWND		hParentWnd = Utility::g_hWnd;

	// 子进程仍然存在?
	char mutex_name[MAX_PATH] = {0};
	sprintf(mutex_name, "Global\\sxsy360wan_pay_webwnd_mutex_%d", (unsigned int)hParentWnd);
	HANDLE hMutex = OpenMutex(MUTANT_QUERY_STATE, FALSE, mutex_name);
	if (hMutex != NULL) {
		CloseHandle(hMutex);
		return 2;
	}

	// 关闭句柄
	hide_external_web_window(false);

	// launch process
	//url = "https://iap.g.360-api.cn/iap.html?uid=3224474221&platform=wan&gkey=sxsy&skey=s1&amount=10";
	StringType cmdline;
	UT_BaseStringUtility::format(cmdline, "/parenthwnd=%d;/windowwidth=%d;/windowheight=%d;/url=%s;/jumpurl=%s;/mutex=%s;", hParentWnd, web_width, web_height, url, jump_url, mutex_name);
	StringType b64 = Base64Encode(cmdline);
	b64.insert(0, "sxsyminipay.exe ");

	PROCESS_INFORMATION pi;
	BOOL bOk = RunExe(NULL, SW_SHOW, NULL, (char *)b64.c_str(), &pi, false);
	if (bOk)
	{
		s_hExternalWebProcess = pi.hProcess;
		return 0;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// 内嵌支付页面，外部支付进程调起失败的时候顶上

static LRESULT CALLBACK cbInternalWebWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		UnEmbedBrowserObject(hWnd);
		s_hInternalWebWnd = NULL;
		::SetForegroundWindow(Utility::g_hWnd);	//< active parent
		break;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}


static int show_internal_web_window( const char * url, int web_width, int web_height, int x=0, int y=0 )
{
	HINSTANCE	hInstance  = Utility::g_hWindowsInst;
	HWND		hParentWnd = Utility::g_hWnd;
	LPCTSTR		pClassName = "mk_JZEmbeddedWebWindow";


	static int s_bRegistedClass = 0;
	if (!s_bRegistedClass)
	{
		s_bRegistedClass = 1;

		WNDCLASSEX wc;
		wc.cbSize           = sizeof(WNDCLASSEX);
		wc.style            = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc      = cbInternalWebWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = 0;
		wc.hInstance        = hInstance;
		wc.hIcon		    = ::LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor          = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground    = 0;
		wc.lpszMenuName     = 0;
		wc.lpszClassName    = pClassName;
		wc.hIconSm		    = NULL;
		::RegisterClassEx(&wc);
	}

	//////////////////////////////////////////////////////////////////////////

	if (s_hInternalWebWnd != NULL)
	{
		::SetForegroundWindow(s_hInternalWebWnd);
	}
	else
	{
		RECT	r = {0};
		::GetWindowRect(hParentWnd, &r);

		if( x == 0 && y == 0)
		{
			//< center pos
			x = (r.left + r.right - web_width) / 2;
			y = (r.top + r.bottom - web_height) / 2;
		}
		
		// adjust window rect from client rect
		RECT	cr = {0, 0, web_width, web_height};
		DWORD style = WS_CHILD | WS_DISABLED;
		AdjustWindowRect(&r, style, FALSE);

		s_hInternalWebWnd = CreateWindowEx(WS_EX_TOOLWINDOW, pClassName, "", style,
			x, y, cr.right - cr.left, cr.bottom - cr.top, hParentWnd, NULL, hInstance, NULL);

		long rs = EmbedBrowserObject(s_hInternalWebWnd);
		if (rs == 0)
		{
			::ShowWindow(s_hInternalWebWnd , SW_SHOW);
			::UpdateWindow(s_hInternalWebWnd);
			DisplayHTMLPage(s_hInternalWebWnd, const_cast<char *>(url));
		}
		else
		{
			::DestroyWindow(s_hInternalWebWnd);
			s_hInternalWebWnd = NULL;
		}
	}

	return s_hInternalWebWnd != NULL ? 0 : 1;
}

static void hide_internal_web_window(void)
{
	if (s_hInternalWebWnd != NULL)
	{
		::DestroyWindow(s_hInternalWebWnd);
		s_hInternalWebWnd = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

bool showWebWindow( const char * url, int web_width, int web_height)
{
	// cd
	DWORD ct = GetTickCount();
	if (ct - s_nWebOpenTime < 1000)
		return false;

	int nCode = show_external_web_window(url, "", web_width, web_height);
	if (nCode == 0)
	{
		s_nWebOpenTime = ct;
		return true;
	}

	if (nCode == 1)
	{
		if (0 == show_internal_web_window(url, web_width, web_height))
		{
			s_nWebOpenTime = ct;
			return true;
		}
	}

	return false;
}

bool showShunWangWebWindow( const char * url, const char* jump_url, int web_width, int web_height)
{
	// cd
	DWORD ct = GetTickCount();
	if (ct - s_nWebOpenTime < 1000)
		return false;

	int nCode = show_external_web_window(url, jump_url, web_width, web_height);
	if (nCode == 0)
	{
		s_nWebOpenTime = ct;
		return true;
	}

	if (nCode == 1)
	{
		if (0 == show_internal_web_window(url, web_width, web_height))
		{
			s_nWebOpenTime = ct;
			return true;
		}
	}

	return false;
}

void hideWebWindow()
{
	s_nWebOpenTime = 0;
	hide_external_web_window(true);
	hide_internal_web_window();
}

bool showInnerWebWindow(const char * url, int web_width, int web_height, int x, int y)
{
	DWORD ct = GetTickCount();
	if (ct - s_nWebOpenTime < 1000)
		return false;

	if (0 == show_internal_web_window(url, web_width, web_height, x, y))
	{
		s_nWebOpenTime = ct;
		return true;
	}

	return false;
}

void resetInnerWebWindow(int width, int height, int x, int y)
{
	if(s_hInternalWebWnd != NULL)
	{
		MoveWindow(s_hInternalWebWnd, x, y, width, height, TRUE);
	}
}

//////////////////////////////////////////////////////////////////////////

bool report2tx_keepalive()
{
	if (getYunYingplatform() == YYP_TX)
	{
		if (!ClientCmdlineArg::HasValue(CMD_ARG_PLATFORM_FULL))	return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_TX_SID))		return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))		return false;	//< openid
		if (!ClientCmdlineArg::HasValue(CMD_ARG_OPENKEY))	return false;	//< openkey

		const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_FULL);
		const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_TX_SID);
		const StringType & c = ClientCmdlineArg::GetValue(CMD_ARG_UID);		//< openid
		const StringType & d = ClientCmdlineArg::GetValue(CMD_ARG_OPENKEY);	//< openkey

		//////////////////////////////////////////////////////////////////////////

		char url[2048] = "";
		sprintf(url,  "\"http://s%s.app1104956040.qqopenapp.com/tencent/getuserInfo.php?infotype=is_login&platform=%s&openid=%s&openid=%s\"",
			b.c_str(), 
			a.c_str(), c.c_str(), d.c_str());

		//////////////////////////////////////////////////////////////////////////

		ShellExecute(0, "open", "curl.exe", url, NULL, SW_HIDE);
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"curl.exe %s", url);	//< 注意url中%问题

		return true;
	}
	return false;
}

bool report2tx(const char *acttype, const char * postfix)
{
	if (!ClientCmdlineArg::HasValue(CMD_ARG_PLATFORM_FULL))	return false;
	if (!ClientCmdlineArg::HasValue(CMD_ARG_TX_SID))		return false;
	if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))			return false;	//< openid

	const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_FULL);
	const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_TX_SID);
	const StringType & c = ClientCmdlineArg::GetValue(CMD_ARG_UID);		//< openid
	const StringType & d = ClientCmdlineArg::GetValue(CMD_ARG_CLIENT_VER);

	//////////////////////////////////////////////////////////////////////////
	const char * version = strrchr(d.c_str(), '.');
	if (version == NULL)
		version = "1";
	else
		version++;

	StringStreamType usm;
	usm << "\"http://s"		<< b.c_str() << ".app1104956040.qqopenapp.com/tencent/userCompass.php?";
	usm << "opttype="		<< acttype;
	usm << "&platform="		<< a.c_str();
	usm << "&serverid="		<< b.c_str();
	usm << "&openid="		<< c.c_str();
	usm << "&version="		<< version;
	usm << postfix;
	usm << "\"";

	// "http://s%s.app1104956040.qqopenapp.com/tencent/userCompass.php?opttype=%s&platform=%s&serverid=%s&openid=%s&version=%s&%s",

	//////////////////////////////////////////////////////////////////////////
	StringType   str = usm.str();
	const char * url = str.c_str();

	ShellExecute(0, "open", "curl.exe", url, NULL, SW_HIDE);
	Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"curl.exe %s", url);	//< 注意url中%问题

	return true;
}

void report2tx_login( object_id_type uid, int level )
{
	if (getYunYingplatform() == YYP_TX)
	{
		StringStreamType s;
		s << "&userid=" << uid;
		s << "&level="	<< level;

		//< union hard code
		if (isTxUnion())
		{
			s << "&reserve_5="	<< ClientCmdlineArg::GetValueSafe(CMD_ARG_PLATFORM_FULL);
		}

		StringType ss = s.str();
		report2tx("login", ss.c_str());
	}
}

void report2tx_quit( object_id_type uid, int level, int onlinetime )
{
	if (getYunYingplatform() == YYP_TX)
	{
		StringStreamType s;
		s << "&userid="		<< uid;
		s << "&level="		<< level;
		s << "&onlinetime=" << onlinetime;

		StringType ss = s.str();
		report2tx("quit", ss.c_str());
	}
}

void report2tx_register( object_id_type uid )
{
	if (getYunYingplatform() == YYP_TX)
	{
		StringStreamType s;
		s << "&userid=" << uid;

		//< union hard code
		if (isTxUnion())
		{
			s << "&reserve_5="	<< ClientCmdlineArg::GetValueSafe(CMD_ARG_PLATFORM_FULL);
		}

		//< invited?
		const StringType & a = ClientCmdlineArg::GetValueSafe(CMD_ARG_APPENDINFO);
		bool bInvite = a.find("invkey") != a.npos;

		StringType ss = s.str();
		report2tx(bInvite ? "accept" : "register", ss.c_str());

		//< extra report for tx union
		if (isTxUnion())
		{
			StringStreamType es;
			es << "&userid="	<< uid;
			es << "&openkey="	<< ClientCmdlineArg::GetValueSafe(CMD_ARG_OPENKEY);
			es << "&pfkey="		<< ClientCmdlineArg::GetValueSafe(CMD_ARG_PFKEY);

			StringType ess = es.str();
			report2tx("union_reg", ess.c_str());
		}
	}
}

void report2tx_consume( object_id_type uid, MoneyType yuanbao )
{
	if (getYunYingplatform() == YYP_TX)
	{
		MoneyType modifyfee = yuanbao * 10;

		StringStreamType s;
		s << "&userid="		<< uid;
		s << "&modifyfee="	<< modifyfee;

		StringType ss = s.str();
		report2tx("consume", ss.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////

bool report2xy( const char *acttype, const char * postfix )
{
	if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))		return false;
	if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))		return false;

	const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_SID);
	const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_UID);

	//////////////////////////////////////////////////////////////////////////

	char url[2048] = "";
	sprintf(url,  "\"http://s%s.jzwl.xiyou.xiyou-g.com:8075/%s?sid=%s&uid=%s&gkey=jzwl&%s\"",
		a.c_str(), acttype,
		a.c_str(), b.c_str(), 
		postfix);

	//////////////////////////////////////////////////////////////////////////

	ShellExecute(0, "open", "curl.exe", url, NULL, SW_HIDE);
	Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"curl.exe %s", url);	//< 注意url中%问题

	return true;
}

//< CreateCharParam.csv
//< 1武当 2昆仑 3少林 4崆峒 5峨嵋 6华山
static const char * s_RoleProfessionMap[] = {
	"xx",
	"yujian",		//< "wudang",
	"zhanren",		//< "kunlun",
	"shaolin",		//< "shaolin",
	"pojun",		//< "kongtong",
	"canglong",		//< "emei",
	"huashan",		//< "huashan",
};

void report2xy_login( const StringType & role_name, int role_level, int role_profession )
{
	if (getYunYingplatform() == YYP_52XIYOU)
	{

		role_profession --;
		if (role_profession < 0 || role_profession >= _countof(s_RoleProfessionMap))
			role_profession = 0;

		const char * prof = s_RoleProfessionMap[role_profession];

		StringStreamType s;
		StringType utf8_name = CodeConvert::MbsToUtf8(role_name);
		s << "name:" << utf8_name.c_str() << "|level:" << role_level << "|profession:" << prof;

		std::string encode_name = jzUrlEncode(s.str().c_str());
		std::string url_param = std::string("data=") + encode_name;

		//http://s9998.jzwl.xiyou.52xiyou.com:8075/report_role?sid=&uid=&gkey=data=name:xxx|level:xxx|profession:xxx

		report2xy("report_role", url_param.c_str());
	}
}
//////////////////////////////////////////////////////////////////////////

void report2360wan_levelup( int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_360WAN)
		return;

//	if (newLevel !=2 && newLevel < 30)
//		return;

	if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))		return;
	if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))		return;

	const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_SID);
	const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_UID);

	//////////////////////////////////////////////////////////////////////////

	role_profession --;
	if (role_profession < 0 || role_profession >= _countof(s_RoleProfessionMap))
		role_profession = 0;
	const char * prof = s_RoleProfessionMap[role_profession];


	//< 原始数据
	StringStreamType usm;
	usm << "\"";
	usm << "http://s"		<< a.c_str();
	usm << ".jzwl.360wan.xiyou-g.com:8075/360wan/reportplayer?";
	usm << "qid="			<< b.c_str();
	usm << "&server_id=S"	<< a.c_str();
//	usm << "&gkey=wlol";
	usm << "&playid="		<< uuid;
	usm << "&level="		<< newLevel;
	usm << "&name="			<< jzUrlEncode(CodeConvert::MbsToUtf8(role_name).c_str()).c_str();
	usm << "&gender="		<< ((role_gender == 1) ? "m" : "w");
	usm << "&toptype=zhanli";
	usm << "&topvalue="		<< role_power;
	usm << "&job="			<< prof;
	usm << "\"";

	//////////////////////////////////////////////////////////////////////////

	StringType   str = usm.str();
	const char * url = str.c_str();

	ShellExecute(0, "open", "curl.exe", url, NULL, SW_HIDE);
	Utility::GfxWriteRawLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, url);
}

void report2360youxi_levelup( int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_360YOUXI)
		return;

//	if (newLevel !=2 && newLevel < 30)
//		return;

	if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))		return;
	if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))		return;

	const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_SID);
	const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_UID);

	//////////////////////////////////////////////////////////////////////////

	role_profession --;
	if (role_profession < 0 || role_profession >= _countof(s_RoleProfessionMap))
		role_profession = 0;
	const char * prof = s_RoleProfessionMap[role_profession];


	//< 原始数据
	StringStreamType usm;
	usm << "\"";
	usm << "http://s"		<< a.c_str();
	usm << ".jzwl.youxi.xiyou-g.com:8075/360yx/reportplayer?";
	usm << "qid="			<< b.c_str();
	usm << "&server_id=S"	<< a.c_str();
//	usm << "&gkey=wlol";
	usm << "&playid="		<< uuid;
	usm << "&level="		<< newLevel;
	usm << "&name="			<< jzUrlEncode(CodeConvert::MbsToUtf8(role_name).c_str()).c_str();
	usm << "&gender="		<< ((role_gender == 1) ? "m" : "w");
	usm << "&toptype=zhanli";
	usm << "&topvalue="		<< role_power;
	usm << "&job="			<< prof;
	usm << "\"";

	//////////////////////////////////////////////////////////////////////////

	StringType   str = usm.str();
	const char * url = str.c_str();

	ShellExecute(0, "open", "curl.exe", url, NULL, SW_HIDE);
	Utility::GfxWriteRawLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, url);
}

//////////////////////////////////////////////////////////////////////////

void report2duowan_getreportable( int sceneId, const StringType & sid, const StringType & rolename )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_DUOWAN)
		return;

	//////////////////////////////////////////////////////////////////////////
	wchar_t		strSceneId[64];
	_itow(sceneId, strSceneId, 10);

	std::wstring w_s = CodeConvert::MbsToWcs(sid);
	std::wstring w_n = CodeConvert::MbsToWcs(rolename);

	w_s.insert(0, 1, 's');			//< sid要带前缀's'

	int r = YG_GAMEMSG::GetReportable(w_n.c_str(), strSceneId, w_s.c_str(), DUOWAN_GKEY);
	YunYingReportArg::singleton().setduowan_reportable(r == 0);	//< 唯一写

	GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_TAG_SYSTEM, "duowan reportable ret=%d", r);
}

void report2duowan_register( unique_id_type uuid, const char * role_name, int role_gender, int role_level, int role_profession )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_DUOWAN)
		return;

	//////////////////////////////////////////////////////////////////////////

	if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))		return;
	if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))		return;

	const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_SID);
	const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_UID);

	role_profession --;
	if (role_profession < 0 || role_profession >= _countof(s_RoleProfessionMap))
		role_profession = 0;
	const char * prof = s_RoleProfessionMap[role_profession];

	//////////////////////////////////////////////////////////////////////////

	int			 uid = atoi(b.c_str());
	std::wstring w_a = CodeConvert::MbsToWcs(a);
	//std::wstring w_b = CodeConvert::MbsToWcs(b);
	std::wstring w_n = CodeConvert::MbsToWcs(role_name);
	std::wstring w_p = CodeConvert::MbsToWcs(prof);

	w_a.insert(0, 1, 's');			//< sid要带前缀's'

	YG_GAMEMSG::YGS_GameProfileParams gpp;
	gpp.strSex		= ((role_gender == 1) ? L"m" : L"f");	//男性:m 女性:f
	gpp.strJob		= w_p.c_str();

	//////////////////////////////////////////////////////////////////////////

	BOOL bReport = YG_GAMEMSG::ReportGameProfile(L"", uid, DUOWAN_GKEY, w_a.c_str(), w_n.c_str(), role_level, L"new_role", gpp);
	Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "duowan ReportGameProfile login(%d)", bReport);
}

void report2duowan_levelup( int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_DUOWAN)
		return;

	//////////////////////////////////////////////////////////////////////////

	if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))		return;
	if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))		return;

	const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_SID);
	const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_UID);

	role_profession --;
	if (role_profession < 0 || role_profession >= _countof(s_RoleProfessionMap))
		role_profession = 0;
	const char * prof = s_RoleProfessionMap[role_profession];

	//////////////////////////////////////////////////////////////////////////

	int			 uid = atoi(b.c_str());
	std::wstring w_a = CodeConvert::MbsToWcs(a);
	//std::wstring w_b = CodeConvert::MbsToWcs(b);
	std::wstring w_n = CodeConvert::MbsToWcs(role_name);
	std::wstring w_p = CodeConvert::MbsToWcs(prof);

	w_a.insert(0, 1, 's');			//< sid要带前缀's'

	YG_GAMEMSG::YGS_GameProfileParams gpp;
	gpp.strSex		= ((role_gender == 1) ? L"m" : L"f");	//男性:m 女性:f
	gpp.strJob		= w_p.c_str();
	gpp.nFightPower = role_power;

	//////////////////////////////////////////////////////////////////////////

	BOOL bReport = YG_GAMEMSG::ReportGameProfile(L"", uid, DUOWAN_GKEY, w_a.c_str(), w_n.c_str(), newLevel, L"level_change", gpp);
	Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "duowan ReportGameProfile levelup(%d)", bReport);
}

void report2duowan_chatmsg( const StringType & chatContent, const StringType & toUserName, const StringType & fromUserName, int bMultipy, int sceneId )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_DUOWAN)
		return;

	if (!YunYingReportArg::singleton().d_duowanChatReportable)	//< 唯一读
		return;

	//////////////////////////////////////////////////////////////////////////

	if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))		return;

	const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_SID);

	//////////////////////////////////////////////////////////////////////////

	std::wstring w_a = CodeConvert::MbsToWcs(a);
	std::wstring w_c = CodeConvert::MbsToWcs(chatContent);
	std::wstring w_t = CodeConvert::MbsToWcs(toUserName);
	std::wstring w_f = CodeConvert::MbsToWcs(fromUserName);

	w_a.insert(0, 1, 's');			//< sid要带前缀's'
	YG_GAMEMSG::YG_MESSAGE_TYPE ymt = bMultipy ? YG_GAMEMSG::emONE_VS_MULTIPLE : YG_GAMEMSG::emONE_VS_ONE;

	wchar_t		strSceneId[64];
	_itow(sceneId, strSceneId, 10);

	//////////////////////////////////////////////////////////////////////////

	BOOL bReport = YG_GAMEMSG::CollectMessage(w_c.c_str(), w_t.c_str(), w_f.c_str(), ymt, strSceneId, w_a.c_str(), DUOWAN_GKEY);
	Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "duowan CollectMessage %d", bReport);
}

void report2duowan_async_getreportable( int sceneId, const char * rolename )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_DUOWAN)
		return;

	MG_MessageCenter::GetSingleton().SendMessage(
		CreateMsg_ADD_DUOWAN_GET_REPORTABLE_TASK(sceneId, ClientCmdlineArg::GetValue(CMD_ARG_SID).c_str(), rolename),
		true);
}

void report2duowan_async_register( const char * role_name, int role_gender, int role_level, int role_profession )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_DUOWAN)
		return;

	//< todo
}

void report2duowan_async_levelup( int newLevel, unique_id_type uuid, const char * role_name, int role_gender, int role_profession, int role_power )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_DUOWAN)
		return;

	MG_MessageCenter::GetSingleton().SendMessage(
		CreateMsg_ADD_DUOWAN_REPORT_LEVELUP_TASK(newLevel, uuid, role_name, role_gender, role_profession, role_power),
		true);
}

void report2duowan_async_chatmsg( const char * chatContent, const char * toUserName, const char * fromUserName, int bMultipy, int sceneId )
{
	YunYingPlatform yyp = getYunYingplatform();
	if (yyp != YYP_DUOWAN)
		return;

	if (!YunYingReportArg::singleton().d_duowanChatReportable)	//< 唯一读
		return;

	static const char * ls_filter[] = {
		"|#@yh|",
		};
	for (int i = 0; i < _countof(ls_filter); ++i)	{
		if (strstr(chatContent, ls_filter[i]) != NULL)
			return;
	}

	MG_MessageCenter::GetSingleton().SendMessage(
		CreateMsg_ADD_DUOWAN_REPORT_CHAT_TASK(chatContent, toUserName, fromUserName, bMultipy, sceneId),
		true);	//< 必须true
}

//////////////////////////////////////////////////////////////////////////
