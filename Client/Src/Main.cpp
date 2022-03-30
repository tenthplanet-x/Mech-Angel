#include "ClientPCH.h"

#include "../Build/resource.h"

#include "Dbglib.h"
#include "ClientApp.h"

#include "UT_ThreadEvent.h"
#include "UT_FileManager.h"
#include "UT_ClientGlobalSettings.h"
#include "RG_SplashScreen.h"
#include "UT_MemTracker.h"
#include "GlobalSettings.h"

#if T_MEMORY_TRACKER_DETOUR
  #include "UT_MemHook.h"
  #pragma init_seg(compiler)
  static Memory::UT_MemHook __mh_initialize_dummy;
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
HWND CreateMainWindow(HINSTANCE hInstance,int iCmdShow);
bool ProcessMessages(void);
void GetWindowParams(bool bIsFullscreen, DWORD& dwExStyle, DWORD& dwStyle, int& nWidth, int& nHeight, int& nOffsetX, int& nOffsetY);

//////////////////////////////////////////////////////////////////////////

ClientApp* spClientApp = NULL;
TRACKMOUSEEVENT *spTme = NULL;
HMODULE			g_hClientModule = NULL;		//hinstance
HWND			g_hMainWnd = NULL;
BOOL			g_bMainWndUnicode = false;	//g_hMainWnd is Unicode Window
int				g_dllRunInBrowser = 1;		//ԭ�й��� ��1:RGClient.dll�������������; 0:��launcher�����С�
											//���й��� ��1:RGClient.dll�����������launcher������; 0: ʹ��RGClient.exeֱ�����С�

int				g_iMainWndWidthChanging = 0;	// ���촰��ʱ�Ļ������
int				g_iMainWndHeightChanging = 0;
bool			g_bIsMainWndSizeChanging = false;
bool			g_bIsMainWndSizeChanged = false;

bool			g_bIsMainWndMinimumed = false;	// ��С������ʱ��Ļ������


//////////////////////////////////////////////////////////////////////////
#ifndef CLIENT_DLL
void	commitPatchProgress(int event, int code)
{
	Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, __FUNCTION__", event=%d, code=%d", event, code);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	g_hClientModule = hInstance;
	g_dllRunInBrowser = 0;
	StringType strCmdline = GetCommandLine();
	ClientCmdlineArg::Assign(strCmdline);

	commitPatchProgress(EVENT_CLIENT_INIT_1, 0);

    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	SystemParametersInfo(SPI_SETKEYBOARDDELAY, 0, 0, SPIF_SENDWININICHANGE);	// ���ü����ӳ���С

#ifndef _SHIPPING
	#define _FP_EXCEPTION
#endif

//#ifdef	_FP_EXCEPTION	//�����������쳣
//	unsigned int fpctrl = _controlfp(0, 0);
//	fpctrl &= ~(_EM_OVERFLOW|_EM_ZERODIVIDE|_EM_INVALID);
//	_controlfp(fpctrl, ~0);
//#endif

	//< dump���ܳ�ʼ��
	StringType strClientVersion = Utility::GetThisAppVersion();
	DbgLib::CDebugFx::s_strVersion = CodeConvert::MbsToWcs(strClientVersion);
	DbgLib::CDebugFx::s_strProduct = L"rgclient";
	DbgLib::CDebugFx::s_dwModuleBaseAddress = (DWORD)::GetModuleHandle(NULL);
	DbgLib::CDebugFx::s_bUploadFile = FALSE;
    DbgLib::CDebugFx::SetExceptionHandler(true);
    Utility::SetThreadName("Client Main Thread");

	//< �ڴ����
#if T_MEMORY_TRACKER || T_MEMORY_TRACKER_DETOUR
	Memory::UT_MemTracker* pkTracker = NULL;
    int nEnableTracker = ::GetPrivateProfileInt("MemoryTracker", "EnableTMemTracker", 0, Utility::GfxMakeFullPath(CLIENT_USER_PROFILE));
    if( nEnableTracker == 1 )
    {
		pkTracker = new Memory::UT_MemTracker(65536, 65536);
    }
	//�����ڴ�й©�õĶϵ�
	::DeleteFile("ActiveMemory.log");
#endif

    //�����ļ�ϵͳ
    UT_FileManager* pkFileManager = T_NEW_D UT_FileManager();

    //�����ļ���ʼ�����������ǰ��
    Utility::UT_GlobalSettings* pkSettings = T_NEW_D UT_ClientGlobalSettings;
	if(!pkSettings->Load())
	{
	   MessageBoxW(0, L"GameSettings.lua Corrupted",L"Error",MB_OK | MB_ICONERROR);
	   goto label_fini_2;
	}

	Utility::GlobalSettings* pkLogicSettings = T_NEW_D GlobalSettings;
	if(!pkLogicSettings->Load())
	{
		MessageBoxW(0, L"GameLogicSettings.lua Corrupted",L"Error",MB_OK | MB_ICONERROR);
		goto label_fini_2;
	}
	commitPatchProgress(EVENT_CLIENT_INIT_2, 0);

	//< mouse track
	spTme = new TRACKMOUSEEVENT();
	spTme->cbSize = sizeof(TRACKMOUSEEVENT);
	spTme->dwFlags = TME_LEAVE;
	spTme->dwHoverTime = HOVER_DEFAULT;

	//< main window
	HWND hWnd = CreateMainWindow(hInstance,SW_HIDE);
	g_hMainWnd = hWnd;
	g_bMainWndUnicode = ::IsWindowUnicode(g_hMainWnd);

	//< client app
	spClientApp = T_NEW_D ClientApp();
	if (!spClientApp->Init(hWnd, hInstance))
		goto label_fini_0;

	//< main loop
	while (ProcessMessages())
	{
		if (spClientApp->m_bInitlizing)
			spClientApp->InitRun();
		else
			spClientApp->Run();
	}

	hideWebWindow();
	/// �����˳�
	::ExitProcess(2333);

label_fini_0:
	//< client app
    spClientApp->UnInit();
    T_SAFE_DELETE(spClientApp);

	//< mouse track
	if(spTme)
	{
		delete spTme;
		spTme = NULL;
	}

label_fini_2:
	//�����ļ�
	T_SAFE_DELETE( pkLogicSettings );
    T_SAFE_DELETE( pkSettings );

	//�����ļ�ϵͳ
    T_SAFE_DELETE( pkFileManager );

	//< �ڴ����
#if T_MEMORY_TRACKER
    delete pkTracker;
#endif

	//< ȫ�ֱ���
	ClientCmdlineArg::Clear();

    return 0;
}

//////////////////////////////////////////////////////////////////////////


HWND CreateMainWindow(HINSTANCE hInstance,int iCmdShow)
{
	WCHAR	 lpszName[32];
	LoadStringW(g_hClientModule, IDS_GAME_NAME, lpszName, sizeof(lpszName));

    WNDCLASSEXW wc;
    wc.cbSize           = sizeof(WNDCLASSEXW);
    wc.style            = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc      = WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = hInstance;
    wc.hIcon		    = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor          = (HCURSOR)::LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ARROW));
    wc.hbrBackground    = NULL;//(HBRUSH)::GetStockObject(GRAY_BRUSH);	ȥ��hbrBackground�Ժ󣬴��ڱ����������Ӧ�ó����Լ�����Win7�£���������������ƹ������ٵ���SplashWindow��˸
    wc.lpszMenuName     = 0;
    wc.lpszClassName    = L"rx_RGameClient";
    wc.hIconSm		    = ::LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON2));
    RegisterClassExW(&wc);

    bool bFullScreen = false;
	int nWidth = 1024, nHeight = 768;
	int nOffsetX = 0, nOffsetY = 0;

	Utility::OnlyGetClientWindowConfig(nWidth, nHeight, bFullScreen);
	GetDesktopCenterParamter(nWidth, nHeight, nOffsetX, nOffsetY);

	HWND hWnd = CreateWindowExW(WS_EX_WINDOWEDGE, wc.lpszClassName, lpszName, WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CAPTION | WS_TILEDWINDOW | WS_SIZEBOX, 
		nOffsetX, nOffsetY, nWidth, nHeight, NULL, NULL, hInstance, NULL);

    ::ShowWindow(hWnd , iCmdShow);
    ::UpdateWindow(hWnd);

    RECT rect;
	GetClientRect(hWnd, &rect);
	GfxWriteLog( LOG_INFO_FILELINE , LOG_SWITCH_INFO, "CreateMainWindow width = %d # height = %d", rect.right - rect.left, rect.bottom - rect.top);

	g_settings.SetWindowWidth(rect.right - rect.left);
    g_settings.SetWindowHeight(rect.bottom - rect.top);

    return hWnd;
}
#endif	// CLIENT_DLL

void SetupMainWindow(HWND hWnd)
{
	bool bFullScreen = false;
    int nWidth = 1024, nHeight = 768;
	int nOffsetX, nOffsetY;

	Utility::OnlyGetClientWindowConfig(nWidth, nHeight, bFullScreen);

    ::ShowWindow(hWnd , SW_SHOWNORMAL);
    ::UpdateWindow(hWnd);

	GetDesktopCenterParamter(nWidth, nHeight, nOffsetX, nOffsetY);

    ::SetWindowPos(hWnd, HWND_TOP, nOffsetX, nOffsetY, nWidth, nHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    RECT rect;
    GetClientRect(hWnd, &rect);
    g_settings.SetWindowWidth(rect.right - rect.left);
    g_settings.SetWindowHeight(rect.bottom - rect.top);
}

void GetDesktopCenterParamter(int& nWidth, int& nHeight, int& nOffsetX, int& nOffsetY)
{
	RECT rt;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rt, 0);

	int nDeskClientW = rt.right - rt.left, 
		nDeskClientH = rt.bottom - rt.top;
	// �����С���������湤�������С
	nWidth = min(nDeskClientW, nWidth);
	nHeight = min(nDeskClientH, nHeight);
	// �����������Ͼ���
	nOffsetX = max((nDeskClientW - nWidth) / 2, 0);
	nOffsetY = max((nDeskClientH - nHeight) / 2, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//��ʼ���׶�
	if (spClientApp && spClientApp->m_bInitlizing)
	{
		if (spClientApp->m_pkSplash)
			if (spClientApp->m_pkSplash->SplashWndProc(hWnd, msg, wparam, lparam))
				return 1;	//RGClient.dll�ڲ�������У����������Ϣ���ܷ���0

		#ifdef CLIENT_DLL
			if (g_dllRunInBrowser)	//�����������
				return 0;
			else
				return DefWindowProcW(hWnd, msg, wparam, lparam);	//launcher������
		#else
			return DefWindowProcW(hWnd, msg, wparam, lparam);	//client��������
		#endif
	}

	//������Ϸ�׶�
    switch (msg) 
    {
    case WM_CLOSE:
        if (spClientApp)spClientApp->ReqestQuit();
        return 1;	//RGClient.dll�ڲ�������У����������Ϣ���ܷ���0
    case WM_DESTROY:
		{
			if (!g_dllRunInBrowser)	//�����������
				PostQuitMessage(0);
		}
        break;
	
	case WM_ENTERSIZEMOVE:
		{
			if ( g_dllRunInBrowser )
			{
				/*	
					ʹ��΢�˻������������ʱ��
					��������Ϣ���ݵ���������
				*/

				// Do nothing!
			}
			else
			{
				g_bIsMainWndSizeChanging = true;
				g_bIsMainWndSizeChanged = false;
			}
		}
		break;
	case WM_SIZING:
		{
			// Do nothing!
		}
		break;
	case WM_SIZE:
		{
			if ( g_dllRunInBrowser )
			{
				/*	
					ʹ��΢�˻��������������Ϸʱ��
					WM_ENTERSIZEMOVE��WM_EXITSIZEMOVE��Ϣ����Ϸ�������޷�����
					���Բ���ʹ����������Ϣtrick���ڳߴ�仯�Ĺ��̡�
				*/

				g_settings.SetWindowWidth(LOWORD(lparam));
				g_settings.SetWindowHeight(HIWORD(lparam));

				if ( spClientApp )
					spClientApp->ProcessWindowsMsg(msg, wparam, lparam, false);
			}
			else
			{
				if ( !g_bIsMainWndSizeChanging )
				{
					if ( ( wparam != SIZE_MINIMIZED ) && 
						!( g_bIsMainWndMinimumed && ( wparam == SIZE_RESTORED || wparam == SIZE_MAXIMIZED ) ) )
					{
						/*
							1.����β���������С������
							2.����β������Ǵ���С���ָ�����
						*/
						g_settings.SetWindowWidth(LOWORD(lparam));
						g_settings.SetWindowHeight(HIWORD(lparam));
					}

					g_bIsMainWndMinimumed = 
						( wparam == SIZE_MINIMIZED );

					if ( spClientApp )
						spClientApp->ProcessWindowsMsg(msg, wparam, lparam, false);
				}
				else
				{
					g_bIsMainWndSizeChanged = true;

					g_iMainWndWidthChanging = LOWORD(lparam);
					g_iMainWndHeightChanging = HIWORD(lparam);
				}
			}
		}   
		break;
	case WM_EXITSIZEMOVE:
		{
			if ( g_dllRunInBrowser )
			{
				/*	
					ʹ��΢�˻������������ʱ��
					��������Ϣ���ݵ�������
				*/
			}
			else
			{
				if ( g_bIsMainWndSizeChanged )
				{
					g_settings.SetWindowWidth(g_iMainWndWidthChanging);
					g_settings.SetWindowHeight(g_iMainWndHeightChanging);

					if ( spClientApp )
						spClientApp->ProcessWindowsMsg(msg, wparam, lparam, false);
				}
				g_bIsMainWndSizeChanging = false;
				g_bIsMainWndSizeChanged = false;
			}
		}
		break;
	
	case WM_DISPLAYCHANGE:
		{
			/// ��Ϊ����ֱ��ʱ��޸ģ���Զ��ʱ����ȫ����Ϸ������ֱ��ʻ��л�����С������С�ڵ�ǰ������Ϸ�ķֱ��ʡ�
			//// �ٴ��л�����ʱ��ϵͳ�Ὣ����ֱ����л�ԭ������ֵ

			if (!g_dllRunInBrowser)
			{
				/// ��ȡ��ǰ��client rect
				UT_Vec3Int kRes = g_settings.GetResolution();
				g_settings.SetWindowWidth(kRes.x);
				g_settings.SetWindowHeight(kRes.y);

				SetupMainWindow(g_hWnd); 
			}
			else
			{
				RECT cr;
				GetClientRect(hWnd, &cr);
				g_settings.SetWindowWidth(cr.right - cr.left);
				g_settings.SetWindowHeight(cr.bottom - cr.top);
			}

			if ( spClientApp )
				spClientApp->ProcessWindowsMsg(msg, wparam, lparam, false);
		}
		break;
	case WM_INIT_COMPLETE:
		spClientApp->InitComplete();
		break;
	case WM_INIT_ON_RESOLUTION_CHANGE:
		{
			RECT cr;
			GetClientRect(hWnd, &cr);
			g_settings.SetWindowWidth(cr.right - cr.left);
			g_settings.SetWindowHeight(cr.bottom - cr.top);

			if ( spClientApp )
				spClientApp->ProcessWindowsMsg(msg, wparam, lparam, false);
		}
		break;
	case WM_IME_CHAR:
		if(spClientApp)
		{
			spClientApp->ProcessWindowsMsg(msg, wparam, lparam);
			return 1;
		}
		break;
	case WM_SETCURSOR:
		return 1;	//����1����������ٴ��������Ϣ���������ָ�����˸
		break;
	case WM_SYSCHAR:////����һЩϵͳ��Ϣ���ݸ�����//////////////////////
		return 1;	//����1����������ٴ��������Ϣ���������ָ�����˸
		break; 
#ifdef _MAP_VIEWER_BUILD
	case WM_CHAR:
		switch ((unsigned char)wparam)
		{
			//case 'q': // terminate the application (ALT+F4 works also)
			//case 'Q':
		case VK_ESCAPE:
			PostMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		}
		break;
#endif
	case WM_KEYUP:

		/// �������������Ϣ��ˢ����ҳ��Ŀǰ����΢�˺�ie�����
		if (wparam == VK_F5)
		{
			StringType strCBWnd = ClientCmdlineArg::GetValueSafe("CALLBACKWND");
			int iCBWnd = UT_BaseStringUtility::toInt(strCBWnd);

			if (iCBWnd != 0)
			{
				std::wstring wstrMessage;
				wstrMessage += L"event=";
				wstrMessage += L"RefreshUrl";
				wstrMessage += L";";
				wstrMessage += L"param=";
				wstrMessage += L"";
				wstrMessage += L";";

				COPYDATASTRUCT	cds;
				cds.dwData = 0x12345678;
				cds.lpData = (PVOID)wstrMessage.c_str();
				cds.cbData = sizeof(WCHAR) * (wstrMessage.length() + 1);
				::SendMessage((HWND)iCBWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)((LPVOID)&cds));
			}
		}

		if(spClientApp)
			spClientApp->ProcessWindowsMsg(msg, wparam, lparam);

		break;
	
   default:
        {
			
			if(msg == WM_MOUSEMOVE && spTme)
			{
				spTme->hwndTrack = hWnd;
				TrackMouseEvent(spTme);
				//break;
			}

			//////////////////////////////////////////////////////////////////////////
			if(msg == WM_RBUTTONDOWN)//���Ƴ�����ʱ����ֹ�����ת
			{
				SetCapture(hWnd);//��Ҫ��ȫ�ֵ�hwnd�Ƚϣ��������������ͻ
			}
			else if(msg == WM_RBUTTONUP)
			{
				ReleaseCapture();
			}
			//////////////////////////////////////////////////////////////////////////
            
            if(spClientApp)
			{
				spClientApp->ProcessWindowsMsg(msg, wparam, lparam);
			}

			if(msg == WM_SYSKEYUP)
			{
				if( (unsigned char)wparam == VK_MENU ||
					(unsigned char)wparam == VK_F10 )
				{
					return 1;	//RGClient.dll�ڲ�������У����������Ϣ���ܷ���0
				}
			}
			
		}
        break;
    }
	
	#ifdef CLIENT_DLL
		if (g_dllRunInBrowser)	//�����������
			return 0;
		else
			return DefWindowProcW(hWnd, msg, wparam, lparam);	//launcher������
	#else
		return DefWindowProcW(hWnd, msg, wparam, lparam);	//client��������
	#endif
}

bool ProcessMessages(void)
{
    MSG message;
    BOOL result = TRUE;

    // Peek at all messages, not just the ones for the main window. If this is not done 
    // the application will be listed as 'not responding' when the main window does not
    // have focus, as it will not receive messages.
    while (result == TRUE)
    {
        // Use PeekMessage() so we can use idle time to render the scene. 
        result = PeekMessage(&message, NULL, 0U, 0U, PM_REMOVE);

        if (result)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);

            if (message.message == WM_QUIT)
            {
                return false;
            }
        }
    }

    return true;
}
