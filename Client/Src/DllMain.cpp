#include "ClientPCH.h"

#ifdef CLIENT_DLL

#include "Dbglib.h"
#include "ClientApp.h"
#include "UT_FileManager.h"
#include "UT_ClientGlobalSettings.h"
#include "RG_SplashScreen.h"
#include "GlobalSettings.h"
#include "tc_interface.h"


extern ClientApp* spClientApp;
extern TRACKMOUSEEVENT *spTme;
extern LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
extern void jzParseCmdline();




BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hClientModule = hModule;
		break;

	case DLL_PROCESS_DETACH:
		g_hClientModule = NULL;

#ifdef USE_TC_MALLOC
		tcmalloc_clearcache();
#endif

		break;
	}
	return TRUE;
}

//打点回调，位于launcher/loader中，客户端中不再存在类似功能，防止冲突
typedef void	(*t_dllPatchProgress)(int event, int code);
t_dllPatchProgress		g_funp_clientCommitPatchProgress = NULL;

void	commitPatchProgress(int event, int code)
{
	if (g_funp_clientCommitPatchProgress)
		g_funp_clientCommitPatchProgress(event, code);

	Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, __FUNCTION__", event=%d, code=%d", event, code);
}

extern "C"
{

	void __declspec( dllexport ) jzldCreate( void* window, unsigned long mode )
	{
		g_hMainWnd = (HWND)window;
		g_bMainWndUnicode = ::IsWindowUnicode(g_hMainWnd);
		commitPatchProgress(EVENT_CLIENT_INIT_1, 0);

		#ifndef _SHIPPING
			//#define _FP_EXCEPTION
		#endif
		
		#ifdef	_FP_EXCEPTION	//开启浮点数异常
			unsigned int fpctrl = _controlfp(0, 0);
			fpctrl &= ~(_EM_OVERFLOW|_EM_ZERODIVIDE|_EM_INVALID);
			_controlfp(fpctrl, ~0);
		#endif

		SystemParametersInfo(SPI_SETKEYBOARDDELAY, 0, 0, SPIF_SENDWININICHANGE);	// 设置键盘延迟最小

		//< dump功能初始化
		StringType mac = UT_BaseStringUtility::SplitString(GetMac(), ';')[0];
		StringType usd = StringType("\nuser : ") + ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_LOGIN) + "_" + ClientCmdlineArg::GetValue(CMD_ARG_UID) + "_" + mac;

		StringType strClientVersion = Utility::GetThisAppVersion(g_hClientModule);
		DbgLib::CDebugFx::s_strVersion = CodeConvert::MbsToWcs(strClientVersion);
		DbgLib::CDebugFx::s_strProduct = L"rgclient";
		DbgLib::CDebugFx::s_dwModuleBaseAddress = (DWORD)g_hClientModule;
		DbgLib::CDebugFx::s_bUploadFile = TRUE;
		DbgLib::CDebugFx::s_strUserData= CodeConvert::MbsToWcs(usd);
		DbgLib::CDebugFx::SetExceptionHandler(true);
		Utility::SetThreadName("Client Main Thread");
	
		//虚拟文件系统
		UT_FileManager* pkFileManager = T_NEW_D UT_FileManager();
	
		//配置文件初始化必须放在最前方
		Utility::UT_GlobalSettings* pkSettings = T_NEW_D UT_ClientGlobalSettings;	//< 应该依赖UT_FileManager
		if(!pkSettings->Load())
		{
			commitPatchProgress(EVENT_CLIENT_ERROR_1, 0);
#ifdef _SHIPPING
			jzUploadFtpFile("setting_fail", ".\\log\\Client.log", true, false);
#endif
			T_ASSERT_CRITICAL_S(false);
		}

		Utility::GlobalSettings* pkLogicSettings = T_NEW_D GlobalSettings;
		if(!pkLogicSettings->Load())
		{
			commitPatchProgress(EVENT_CLIENT_ERROR_1, 0);
#ifdef _SHIPPING
			jzUploadFtpFile("setting_fail", ".\\log\\Client.log", true, false);
#endif
			T_ASSERT_CRITICAL_S(false);
		}

		commitPatchProgress(EVENT_CLIENT_INIT_2, 0);

		//< mouse track
		spTme = new TRACKMOUSEEVENT();
		spTme->cbSize = sizeof(TRACKMOUSEEVENT);
		spTme->dwFlags = TME_LEAVE;
		spTme->dwHoverTime = HOVER_DEFAULT;

		//< main window
		HINSTANCE hInstance = g_hClientModule;
		RECT rect;
		if (GetClientRect(g_hMainWnd, &rect))
		{
			g_settings.SetWindowWidth(rect.right - rect.left);
			g_settings.SetWindowHeight(rect.bottom - rect.top);
		}

		/// 微端版本，获取最上层父窗口，与游戏窗口建立连接关系，处理窗口焦点自动激活的问题
		StringType c = ClientCmdlineArg::GetValueSafe(CMD_ARG_CLIENT_TYPE);
		if (c == "pc")
		{
			StringType strAncestorWnd = ClientCmdlineArg::GetValueSafe(CMD_ARG_ANCESTOR_WND);
			HWND hAncestorWnd = (HWND)UT_BaseStringUtility::toInt(strAncestorWnd);
			if (hAncestorWnd != NULL)
				::PostMessage(hAncestorWnd, WM_APP + 0x426, (WPARAM)g_hMainWnd, 0);
		}

		//< client app
		spClientApp = T_NEW_D ClientApp();	//< 需要用到g_settings
		spClientApp->Init(g_hMainWnd, hInstance);
	}

	void __declspec( dllexport ) jzldDestroy( )
	{
		hideWebWindow();
#if	1
		char	process_path[1024] = {0};
		::GetModuleFileName(0, process_path, 1024);
		if (strstr(process_path, "jlg_sxsy_loader.exe") != NULL)
			::ExitProcess(2333);
#endif


		g_funp_clientCommitPatchProgress = NULL;

		//< client app
		if (spClientApp)
		{
			spClientApp->UnInit();
			T_DELETE_D spClientApp;
			spClientApp = NULL;
		}

		//< mouse track
		if(spTme)
		{
			delete spTme;
			spTme = NULL;
		}

		//配置文件
		Utility::UT_GlobalSettings* pkSettings = &g_settings;
		T_SAFE_DELETE( pkSettings );
		Utility::GlobalSettings* pkLogicSettings = Utility::GlobalSettings::GetSingletonPtr();
		T_SAFE_DELETE( pkLogicSettings );

		//虚拟文件系统
		UT_FileManagerBase* pkFileManager = UT_FileManager::GetSingletonPtr();
		T_SAFE_DELETE( pkFileManager );

		g_hMainWnd = NULL;

		//< 全局变量
		ClientCmdlineArg::Clear();
	}

	void __declspec( dllexport ) jzldSetup( const wchar_t* name, const wchar_t* value )
	{
		/*	"cmdline"
			"inBrowser"
		*/
		if (wcsicmp(name, CMD_ARG_CMDLINE) == 0)
		{
			StringType strMbcs = CodeConvert::WcsToMbs(value);
			ClientCmdlineArg::Assign(strMbcs);
		}
		else if (wcsicmp(name, CMD_ARG_COM_ARG) == 0)
		{
			StringType kv = CodeConvert::WcsToMbs(value);
			TStringVector kv_pair = UT_BaseStringUtility::SplitString(kv, '=');
			if (kv_pair.size() == 2)
				ClientCmdlineArg::SetKV(kv_pair[0], kv_pair[1]);
		}
		else if (wcsicmp(name, CMD_ARG_PATCH_CB) == 0)
		{
			g_funp_clientCommitPatchProgress = t_dllPatchProgress(value);
		}
		else if (wcsicmp(name, CMD_ARG_FLASH_OBJ) == 0)
		{
			RG_SplashWindow::ms_pExternalFlashPlayer = (IJZFlashPlayer *)((DWORD)value);
		}
	}

	void __declspec( dllexport ) jzldUpdate( unsigned long elapse )
	{
		if (spClientApp->m_bInitlizing)
			spClientApp->InitRun();
		else
			spClientApp->Run();
	}

	void __declspec( dllexport ) jzldRender( )
	{
	}

	unsigned int __declspec( dllexport )  jzldMessage( unsigned long msgid, unsigned long wparam, unsigned long lparam )
	{
		if (msgid == WM_CHAR && g_bMainWndUnicode)	//处理JZLauncher.exe(U)中运行，输入法失效
		{
			WndProc(g_hMainWnd, msgid, wparam, lparam);
			return 1;	//处理完输入法，返回1，插件将不再处理此条消息；否则导致输入文字重复出现
		}

		return WndProc(g_hMainWnd, msgid, wparam, lparam);
	}

	float __declspec( dllexport ) jzldGetProgress( )
	{
		if (spClientApp)
		{
			RG_SplashWindow * splash = spClientApp->GetSplashWindow();
			if (splash)
				return splash->GetShowProgress() / 100.0f;
		}
		return 1;
	}
}

#endif