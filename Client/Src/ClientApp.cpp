//------------------------------------------------------------------------------

/*
	!! 必读 !!

	每当增删管理对象指针变量时，勿忘：
	(1) 在 _initPointers() 内置空对象指针；
	(2) 在 _initManagers() 中创建对象；
	(3) 在 _uninitManger() 中销毁对象！
	并保持各对象指针的顺序，避免遗漏或重复。
*/


//------------------------------------------------------------------------------
#include "ClientPCH.h"
#include "ClientApp.h"

#include "MG_MessageQueue.h"
#include "ClientConfigManager.h"
#include "SM_GameStageMachine.h"
#include "GD_ClientAsyncModelManager.h"
#include "GD_DynamicCollisionManager.h"
#include "RG_ClientPointLightManager.h"
#include "RG_SplashScreen.h"
#include "SC_ClientSkillInterface.h"
#include "SC_ClientScriptInterface.h"
#include "PF_ClientPathManager.h"
#include "GlobalSettings.h"
#include "UT_ClientGlobalSettings.h"
#include "IP_ShortcutKeyCode.h"
#include "SM_WalkTransferManager.h"

#include "SK_SP.h"
#include "CSV_Client_Helper.h"
#include "LC_ClientLogicAreaManager.h"
#include "LC_ItemSuitManager.h"
#include "LC_GuildRaidDataManager.h"
#include "LC_MasterBlissManager.h"
#include "LC_HelperDiff.h"
#include "LC_ClientHelp.h"
#include "LC_SkillBookAsset.h"
#include "UT_Pick_Manager.h"

#include "NiStandardAllocator.h"
#include "NiWin32DDSReader.h"
#include "NiWin32TGAReader.h"

#include <fstream>
#include <iomanip>
#include <process.h>
//------------------------------------------------------------------------------
using namespace std;
using namespace Utility;
using namespace Base;
using namespace Terrain;
using namespace CSVFile;
using namespace StateMachine;
using namespace Path;
using namespace Skill;
using namespace Protocol;
using namespace GameLogic;
using namespace Script;
using namespace Camera;
//------------------------------------------------------------------------------
NiInitOptions*  ClientApp::s_pkNiInitOptions    = NULL;
NiAllocator*    ClientApp::s_pkNiMemTracker     = NULL;
//------------------------------------------------------------------------------
UT_LogManager*	 Init_ClientLogManager();

#ifdef _RELEASE
#	define SHOW_MEMORY_INFO
#endif

#ifdef SHOW_MEMORY_INFO
	static size_t log_memory_status(const char * deco, const char * name, size_t * pValueOnEnter = NULL)
	{
		static int s_idx = 1;
		static size_t s_stPrevWorkingSet = 0;

		size_t stSystemWorkingSet;
		size_t stSystemPeekSize;
		UT_StatisticsManager::SampleMemoryInfo(stSystemWorkingSet, stSystemPeekSize);

		int    stInc = ((int)stSystemWorkingSet - (int)s_stPrevWorkingSet) >> 10;
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, 
			"%d:  [working set] = %d KB, inc = %d KB, %s %s", 
			s_idx++, stSystemWorkingSet >> 10, stInc, deco, name);

		s_stPrevWorkingSet = stSystemWorkingSet;
		return stSystemWorkingSet;
	}
	struct __memory_status_helpper
	{
		__memory_status_helpper(const char * str)		{
			m_pName = str;
			m_stCurWorkingSet = log_memory_status("pre ", m_pName);
		}
		~__memory_status_helpper()		{
			log_memory_status("post", m_pName, &m_stCurWorkingSet);
		}

		const char * m_pName;
		size_t m_stCurWorkingSet;
	};
#	define LOG_MEMORY_INFO(deco, name)		log_memory_status(deco, name);
#   define LOG_MEMORY_HELPPER(name)			__memory_status_helpper __mem_status_obj(name);
#else
#	define LOG_MEMORY_INFO(deco, name)		((void)0);
#	define LOG_MEMORY_HELPPER(name)			((void)0);
#endif

//------------------------------------------------------------------------------
ClientApp::ClientApp()
{
	_initPointers();
	m_bWindowsActive = false;
	m_bInitlizing = true;
	m_hInitThread = NULL;

	m_pkLogManager = Init_ClientLogManager();	// 日志管理
}
//------------------------------------------------------------------------------
ClientApp::~ClientApp()
{
}
//------------------------------------------------------------------------------
bool ClientApp::_initEngineBase()
{
	// 配置
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化配置管理");
	{
		m_pkConfigManager = T_NEW_D UT_ConfigManager;
		m_pkConfigManager->Init();
		m_pkConfigManager->RefreshConfigLevel();
	}

	if (m_pkConfigManager->GetAutoConfigLevel() == -1)
	{
		MessageBoxA2W_IDS(IDS_POOR_GRAPHICS_CARD, L"Error", MB_OK|MB_ICONERROR);
		return false;
	}
	if ( g_settings.IsValid() )
	{
		g_settings.UpdateSettings();
	}

	// 必备系统
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化必备系统");
	{
		m_pkProfileManager	= T_NEW_D UT_ProfileManager;

		m_pkClassRegistry	= T_NEW_D UT_ClassRegistry;
		
		m_pkWorldConfig		= T_NEW_D TS_WorldConfig;
		
	}

	return true;
}


//------------------------------------------------------------------------------
bool ClientApp::_initRenderModule()
{
	T_PROFILE_TIMER("初始化渲染器");
	LOG_MEMORY_HELPPER(__FUNCTION__);

	// 初始化RenderManager
	{
		m_pkRenderManager = T_NEW_D RG_RenderManager;
		if ( !m_pkRenderManager->Init() )
		{
			MessageBoxA2W_IDS(IDS_RENDERER_CREATE_FAILED,L"Error",MB_OK|MB_ICONERROR);
			return false;
		}
	}

	return true;
}
//------------------------------------------------------------------------------
bool ClientApp::_initGamebryo()
{
	if ( m_pkSplash )	m_pkSplash->SetSlideProgress( 1 , CLIENTAPP_FLASH_TEXT_1);

	T_PROFILE_TIMER("初始化GB");
	NiRefObject::GetTotalObjectCount();
	int nEnableTracker = ::GetPrivateProfileInt( "MemoryTracker" , "EnableGBMemTracker" , 0 , ( CLIENT_USER_PROFILE ) );
	int nEnableLog = ::GetPrivateProfileInt( "MemoryTracker" , "EnableGBMemLog" , 0 , ( CLIENT_USER_PROFILE ) );
	bool bTrack = ( nEnableTracker == 1 ? true : false );
	//Shipping下打开内存追踪会造成崩溃
#ifndef _SHIPPING
	if ( bTrack )
	{
		// 在 Debug 模式下把 GB 的内存分配全部重定位到 T6 的 Memory 模块中去，实现内存泄漏跟踪的统一
		s_pkNiMemTracker = NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(),  nEnableLog == 1 ? true : false );
	}
	else
	{
		s_pkNiMemTracker = NiExternalNew NiStandardAllocator();
	} 
#else
	s_pkNiMemTracker = NiExternalNew NiStandardAllocator();
#endif
	s_pkNiInitOptions = NiExternalNew NiInitOptions(s_pkNiMemTracker);
	NiInit(s_pkNiInitOptions);

	NiDevImageConverter* pkDevImageConverter = NiNew NiDevImageConverter;
	pkDevImageConverter->AddReader(NiNew NiWin32DDSReader);
	pkDevImageConverter->AddReader(NiNew NiWin32TGAReader);
	NiImageConverter::SetImageConverter(pkDevImageConverter);
	NiTexture::SetMipmapByDefault(false);

	bool bOutput = g_settings.PROFILE_GB_WARNING;
	NiLogger::SetOutputToDebugWindow(0, bOutput);
	NiLogger::SetOutputToDebugWindow(1, bOutput);
	NiLogger::SetOutputToDebugWindow(2, bOutput);

	return true;
}
//------------------------------------------------------------------------------
bool ClientApp::_uninitGameBryo()
{
	NiShutdown();

	unsigned int uiFinalCount = NiRefObject::GetTotalObjectCount();
	unsigned int uiInitialCount = 0;

	char acMsg[256];
	NiSprintf(acMsg, 256,
		"\nGamebryo NiRefObject counts:  initial = %u, final = %u. ", 
		uiInitialCount, uiFinalCount);
	OutputDebugString(acMsg);
	if (uiFinalCount > uiInitialCount)
	{
		unsigned int uiDiff = uiFinalCount - uiInitialCount;
		NiSprintf(acMsg, 256, "Application is leaking %u objects\n\n", uiDiff);
		OutputDebugString(acMsg);
	}
	else
	{
		OutputDebugString("Application has no object leaks.\n\n");
	}

	NiExternalDelete s_pkNiInitOptions;   s_pkNiInitOptions   = NULL;
	NiExternalDelete s_pkNiMemTracker;    s_pkNiMemTracker    = NULL;
	return true;
}
//------------------------------------------------------------------------------
bool ClientApp::CreateSplashWindow(HWND hWnd, HINSTANCE hInst)
{
	LOG_MEMORY_HELPPER(__FUNCTION__);

	m_pkSplash = T_NEW_D RG_SplashWindow(hWnd);
	if ( m_pkSplash && m_pkSplash->isInitlized() )
	{
		m_pkSplash->Show();
		m_pkSplash->Play();
		return true;
	}
	else
	{
		T_SAFE_DELETE( m_pkSplash );
	}

	return false;
}
//------------------------------------------------------------------------------
bool ClientApp::Init( HWND hWnd, HINSTANCE hInst )
{
	m_hWnd = hWnd;
	m_hInst = hInst;
	g_hWnd = hWnd;
	g_hWindowsInst = (HMODULE)(LONG_PTR)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	this->CreateSplashWindow(hWnd, hInst);

	DWORD dwMainThreadID = GetCurrentThreadId();
	GfxWriteLog(LOG_TAG_STAT, LOG_SWITCH_STAT, "MainThreadID:%d",dwMainThreadID);

	//初始化引擎
	if ( !_initEngineBase() )
	{
		commitPatchProgress(EVENT_CLIENT_ERROR_2, 0);
		return false;
	}

	//初始化gamebryo
	if ( !_initGamebryo() )
	{
		commitPatchProgress(EVENT_CLIENT_ERROR_3, 0);
		return false;
	}

	//初始化渲染模块
	if ( !_initRenderModule() )
	{
		commitPatchProgress(EVENT_CLIENT_ERROR_4, 0);

#ifdef _SHIPPING  
		jzUploadFtpFile("CreateRender_Failed_v1_2", "log\\Client_Render.log", true, false);
#endif
		return false;
	}

	commitPatchProgress(EVENT_CLIENT_INIT_3, 0);
	m_hInitThread = (HANDLE)_beginthreadex(NULL, 0, AsyncInitThread, (void *)this, 0, NULL);

	return true;
}

unsigned int __stdcall ClientApp::AsyncInitThread(void *p)
{
	T_PROFILE_TIMER("初始化 客户端资源");

	ClientApp	*pThis = (ClientApp *)p;

	//实时下载和后台下载，放在SplashWindow后面，下载rtcatalog会可能花费较长时间，否则低网速情况下看起来点了客户端没反应
	//2015.03.27： 现在rtcatalog在更新客户端初始包时更新了，这里不会花费较长时间
#ifdef USE_DOWNLOADER
	if (g_settings.ENABLE_DOWNLOAD)
	{
		if ( pThis->m_pkSplash )
		{
			pThis->m_pkSplash->SetSlideProgress( 0 , CLIENTAPP_FLASH_TEXT_2);
			pThis->m_pkSplash->GetProgressArg().m_bDownloadVisible = true;
		}

		static IDownloaderInitArg::ExtConfig  extCfg[] = {
			{"ext_catalog.txt",		true,	false,	false, false, false},
			{"reload_catalog.txt",	true,	true,	true , true,  true},
			{NULL, false, false, false, false},
		};
		IDownloaderInitArg	ia;
		ia.remote_client_ver	= ClientCmdlineArg::GetValue(CMD_ARG_CLIENT_VER).c_str();
		ia.client_url_root		= ClientCmdlineArg::GetValue(CMD_ARG_CLIENT_ROOT_URL).c_str();
		ia.ext_cfgs = extCfg;

		if (!UT_FileManager::GetSingletonPtr()->CreateDownload(&ia))
		{
			commitPatchProgress(EVENT_CLIENT_ERROR_5, 0);

#ifdef _SHIPPING
			jzUploadFtpFile("downloader_err", "log\\downloader_%pid.log", true, false);
#endif
			MessageBoxA2W_IDS(IDS_DOWNLOADER_INIT_FAILED,L"Error",MB_OK);
			T_ASSERT_CRITICAL_S(false);
			return 99;
		}
		if (pThis->m_pkSplash)
			pThis->m_pkSplash->GetProgressArg().m_bDownloadVisible = false;
	}
#endif

	int iSec = g_settings.FILE_LOG_CLEAR_TIME;
	jzDelOldFile(STR_LOGFILE_FILTER, iSec);
	jzDelOldFile(STR_UI_LOGFILE_FILTER, iSec);
	jzDelOldFile(STR_GUILDPROFIT_DIR_FILTER, iSec);
#ifdef _SHIPPING
	jzDelOldFile(STR_LOADER_LOGFILE_FILTER, iSec);
	jzDelOldFile(STR_DOWNLOADER_LOGFILE_FILTER, iSec);
#endif
	int iDay = GetGlobalSetting.BattlePlayBackDeleteDay;
	jzDelOldFile(STR_ARENAPLAYBACK_DIR_FILTER, iDay*24*60*60);

	commitPatchProgress(EVENT_CLIENT_INIT_4, 0);
	{
		LOG_MEMORY_HELPPER("m_pkRenderManager->PostInit");
		pThis->m_pkRenderManager->PostInit();
	}
	// 这个需要在Renderer初始化以后才能采集
	pThis->m_pkConfigManager->RefreshGPUCapsInfo();
	if (!g_settings.IsValid())
	{
		//当用户第一次安装时需要确定一次合适的配置
		//对于不在显卡列表中但是ShaderVersion又很高的显卡当新卡来处理
		pThis->m_pkConfigManager->RefreshConfigLevel();
		pThis->m_pkConfigManager->ApplyConfigLevel();
		g_settings.RestoreDefaultProfile();
		g_settings.UpdateSettings();
		g_settings.SaveGPUConfigLevel();

		// shader中的 g_bShadowTech 变量决定着物件地面，是否在计算ShadowMap，
		// PostLoad()函数会吧g_settings中的	SHADOW_BLEND_FAKE 变量传入到shader中
		// 如果UserProfile2.ini文件不存在，	SHADOW_BLEND_FAKE一开始就没办法被赋值，Render创建好以后，SHADOW_BLEND_FAKE才被赋值，但是这次没有被传入shader中
		// 导致UserProfile2.ini文件不存在，第一次进入游戏没有影子，重新打开游戏就有影子了
		g_settings.PostLoad();
	}
	else
	{
		g_settings.UpdateSettings();
	}

	pThis->_initManagers();
	pThis->_registerMessageHandler();

	if (!g_dllRunInBrowser)
	{
		HWND hMainWnd = pThis->m_hWnd;
		SetupMainWindow(hMainWnd);

		HWND hForeWnd = GetForegroundWindow();
		int iForeTID  = GetWindowThreadProcessId(hForeWnd, 0);
		int iCurrTID  = GetCurrentThreadId();

		AttachThreadInput(iCurrTID, iForeTID, TRUE);
		SetForegroundWindow(hMainWnd);
		AttachThreadInput(iCurrTID, iForeTID, FALSE);
		SetWindowPos(hMainWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW /*| SWP_FRAMECHANGED*/);
	}

	pThis->Start();

	//terminate thread
	pThis->m_bInitlizing = false;
	::PostMessage(pThis->m_hWnd, WM_INIT_COMPLETE, 0, 0);
	::PostMessage(pThis->m_hWnd, WM_INIT_ON_RESOLUTION_CHANGE, 0, 0);
	commitPatchProgress(EVENT_CLIENT_INIT_OK, 0);
	return 0;
}
//------------------------------------------------------------------------------
void ClientApp::InitRun()
{
	if (this->m_pkSplash)
	{
		this->m_pkSplash->Run();

	#ifdef	USE_DOWNLOADER
		IDownloader* downloader = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
		if (downloader != NULL)
		{
			SplashInfo & arg = this->m_pkSplash->GetProgressArg();

			DownloadStatistics s;
			downloader->getWholeStatistics(s);
			arg.m_DownloadSize = s.d_Size;
			arg.m_DownloadSpeed = s.d_Speed;
		}
	#endif
	}
}
//------------------------------------------------------------------------------
void ClientApp::InitComplete()
{
	T_SAFE_DELETE( this->m_pkSplash );
	m_pkUIManager->PostInit();

//#define _FILTER_SENSE_WORD
#ifdef _FILTER_SENSE_WORD
	TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
	if (pkTextFilter)
	{
		std::ifstream  raw_sense_word("../Server/sql/d_jz_random_name.sql", std::ifstream::in);
		if (!raw_sense_word.bad())
		{
			StringStreamType ss;
			ss << raw_sense_word.rdbuf();
			raw_sense_word.close();
			StringType raw_sense;
			Utility::Utf8ToGb2312(ss.str(), raw_sense);
			pkTextFilter->SetText(raw_sense);

			pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAR_NAME);
			
			StringType final_sense;
			Utility::Gb2312ToUtf8(pkTextFilter->GetText(), final_sense);

			std::ofstream final_sense_word("../Server/sql/f_jz_random_name.sql", std::ifstream::out);
			final_sense_word << final_sense;
			final_sense_word.flush();
			final_sense_word.close();
		}
	}
#endif

	//< 释放线程句柄
	if (m_hInitThread != NULL)
	{
		::CloseHandle(m_hInitThread);
		m_hInitThread = NULL;
	}
}
//------------------------------------------------------------------------------
bool ClientApp::UnInit()
{
	//< 等待初始化线程结束
	if (m_hInitThread != NULL)
	{
		::WaitForSingleObject(m_hInitThread, INFINITE);
		::CloseHandle(m_hInitThread);
		m_hInitThread = NULL;
	}

	T_SAFE_DELETE( this->m_pkSplash );

	if (m_pkConfigManager)
	{
		m_pkConfigManager->UnInit();
		T_SAFE_DELETE(m_pkConfigManager);
	}

	_uninitManger();
	_uninitGameBryo();

	return true;
}



//------------------------------------------------------------------------------
void ClientApp::_initPointers()
{
	m_pkSplash                  = NULL;

	m_pkLogManager              = NULL;

	m_pkConfigManager			= NULL;

	m_pkDirConfig               = NULL;

	m_pkClassRegistry           = NULL;

	m_pkLuaStateFactory         = NULL;
	m_pkScriptInterface         = NULL;
	m_pkScriptActionManager		= NULL;

	m_pkProfileManager          = NULL;

	m_pkHelperDiffManager		= NULL;

	m_pkPathManager             = NULL;

	m_pkWorldConfig             = NULL;

	m_pkModelContentManager     = NULL;

	m_pkMaterialManager         = NULL;

	m_pkBreakModelManager		= NULL;

	m_pkStateFactory            = NULL;

	m_pkControllerFactory		= NULL;

	m_pkMessageCenter           = NULL;

	m_pkInputManager            = NULL;
	m_pkShortcutKeyCodeMgr      = NULL;

	m_pkNetManager              = NULL;
	m_pkSessionManager          = NULL;

	m_pkTimer                   = NULL;
	m_pkTimerManager			= NULL;

	m_pkRenderManager           = NULL;

	m_pkCameraManager           = NULL;
	m_pkCameraAnimManager       = NULL;

	m_pkSceneManager            = NULL;
	m_pkPointLightManager       = NULL;

	m_pkTooltipManager			= NULL;
	m_pkUIManager               = NULL;
	m_pkUiEffectManager			= NULL;
	m_pkAvatarManager			= NULL;

	m_pkMapMananger             = NULL;

	m_pkNewBeeManager			= NULL;

	m_pkLogicManager            = NULL;

	m_pkAsyncModelManager       = NULL;
	m_pkClientActorOperationManager	= NULL;

	m_pkIMManager               = NULL;
	m_pkDynamicCollisionManager = NULL;

	m_pkBulletManager           = NULL;
	m_pkPickMeshManager         = NULL;

	m_pkFXManager               = NULL;

	m_pkSkillFactory            = NULL;

	m_pkDebugCmdParser          = NULL;

	m_pkItemFactory             = NULL;

	m_pkTaskManager				= NULL;

	m_pkLogicIDManager          = NULL;

	m_pkLogicMapManager			= NULL;

	m_pkTextManager             = NULL;

	m_pkTextFilter              = NULL;

	m_pkSoundManager            = NULL;

	m_pkClientShopCity			= NULL;

	m_pkAuctionManager			= NULL;

	m_pkGuildManager			= NULL;

	m_pkGuildHomeMgr			= NULL;

	m_pkGuildRaidDataManager	= NULL;

	m_pkBackLoadingProcess      = NULL;

	m_pkLogicTriggerManager     = NULL;

	m_pkDialogManager			= NULL;

	m_pkNPCShopManager			= NULL;

	m_pkConversationManager		= NULL;

	m_pkMapTranscriptionManager	= NULL;

	m_pkUICutScenesManager		= NULL;

	m_pkNPCTransferManager		= NULL;

	m_pkAchievementManager		= NULL;

	m_pkWalkTransferManager		= NULL;

	m_pkNoticeAnalysisManager	= NULL;

	m_pkMailManager				= NULL;

	m_pkCutSceneMgr             = NULL;

	m_pkNPCInteractionManager	= NULL;

	m_pkFactionManager			= NULL;

	m_pkActorFilterManager		= NULL;

	m_pkArenaManager			= NULL;

	m_pkTitleManger				= NULL;

	//m_pkClientCurrencyTradeManager = NULL;

	m_pkPackEntryFactory		= NULL;
	m_pkVIPCmnManager			= NULL;

	m_pkCommonStatManager		= NULL;

	m_pkTreasureChestsManager	= NULL;
	m_pkCommonShopManager		= NULL;
	m_pkAutoFitNPCManager		= NULL;

	m_pkConfigReloadManager		= NULL;

	m_pkSoulRefineManager		= NULL;
	m_pkSoulRefineDataAsset		= NULL;
	m_pkShotActivityManager		= NULL;
	m_pkYunYingManager			= NULL;
	m_pkActivityManager			= NULL;

	m_pkMeltDataManager			= NULL;
	m_pkBoneSoulManager			= NULL;
	m_pkStarsRuneManager		= NULL;
	m_pkMillionWeaponManager	= NULL;

	m_pkPetManager				= NULL;
	m_pkCollectionManager		= NULL;
	m_pkBotManager				= NULL;
	m_pkCombatRecordManager		= NULL;
	m_pkAchievementManager_Common=NULL;
	m_pkGuildProfitManager		= NULL;
	m_pkArenaPlayBackManager	= NULL;
	m_pkReportManager			= NULL;
}
//------------------------------------------------------------------------------
bool ClientApp::_initManagers()
{
	LOG_MEMORY_HELPPER(__FUNCTION__);
	{
		LM_LocaleManager *pkLocaleMgr = T_NEW_D LM_LocaleManager;
		pkLocaleMgr->Init(CLIENT_LOCALEMSG_DIR);
	}

	if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 2, CLIENTAPP_FLASH_TEXT_3);
	// CSV
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化 CSV 管理");
	{
		T_PROFILE_TIMER("初始化 CSV");
		LOG_MEMORY_HELPPER("load_csv");

		if(Init_Client_CSV_File() == false)
		{
			commitPatchProgress(EVENT_CLIENT_ERROR_6, 0);
			T_ASSERT_CRITICAL_S(false);
			return false;
		}
	}

	commitPatchProgress(EVENT_CLIENT_INIT_5, 0);
	//武林盟主全服加成数据
	{
		LC_MasterBlissManager* pkVal = T_NEW_D LC_MasterBlissManager;
		pkVal->Init();
	}

	{
		LC_SkillBookManager* pkMgr = T_NEW_D LC_SkillBookManager;
		pkMgr->Init();
	}

	{
		m_pkHelperDiffManager = T_NEW_D LC_HelperDiffManager;
		LC_HelperDiff* hd = T_NEW_D LC_ClientHelpDiff;
		m_pkHelperDiffManager->SetDiff(hd);
	}

	// 目录
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化目录管理");
	{
		m_pkDirConfig                   = T_NEW_D UT_DirConfig;
		m_pkDirConfig->Init(CF_DIR_CONFIG_FILE_NAME);
	}

	// 特效
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化特效管理");
	{
		m_pkFXManager                   = T_NEW_D FX_ClientFxManager();
	}

	// 必备系统
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化必备系统管理");
	{
		T_PROFILE_TIMER("初始化基础系统");

		m_pkLuaStateFactory             = T_NEW_D LS_LuaStateFactory;

		m_pkScriptInterface             = T_NEW_D SC_ClientScriptInterface;
		m_pkScriptInterface->RegisterToLua();

		m_pkMessageCenter               = T_NEW_D MG_MessageCenter;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化寻路管理");
		{
			m_pkPathManager				= T_NEW_D PF_ClientPathManager;
			m_pkPathManager->Init();
		}

		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化材质管理");
		{
			m_pkMaterialManager				= T_NEW_D BS_ClientMaterialManager;
			m_pkMaterialManager->Init();
		}

		{
			m_pkBreakModelManager			= T_NEW_D BM_Manager;
			m_pkBreakModelManager->Init();
		}

		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化模型管理");
		{
			m_pkModelContentManager		= T_NEW_D BS_ClientModelContentManager;
			m_pkModelContentManager->Init();

			m_pkAsyncModelManager           = T_NEW_D GD_ClientAsyncModelManager;
		}

		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化状态机管理");
		{
			m_pkStateFactory			= T_NEW_D SM_ClientStateFactory;
			m_pkStateFactory->Init();

			m_pkControllerFactory		= T_NEW_D SM_ClientControllerFactory;
			m_pkControllerFactory->Init();
		}

		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化网络管理");
		{
			m_pkSessionManager          = T_NEW_D NW_ClientSessionManager;

			m_pkNetManager				= T_NEW_D NW_ClientNetManager;
			m_pkNetManager->Init();
		}

		if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 3, CLIENTAPP_FLASH_TEXT_4);

		m_pkTimer						= T_NEW_D TM_ClientTimeManager;
		m_pkTimerManager				= T_NEW_D UT_TimerManager;

		m_pkDebugCmdParser              = T_NEW_D UT_DebugCommandParser;
		m_pkDebugCmdParser->SetAllowDebugConsol(true);

		m_pkTextManager					= T_NEW_D TT_TextManager;
		m_pkTextManager->Init();

		// 地图管理
		{
			m_pkMapMananger             = T_NEW_D LC_MapManager;
			m_pkMapMananger->Init();
		}
		//分流管理
		{
			m_pkNewBeeManager			= T_NEW_D LC_ClientNewBeeManager;
			m_pkNewBeeManager->Init();
		}

		m_pkTextFilter					= T_NEW_D TT_TextFilter;

		m_pkDialogManager				= T_NEW_D LC_ClientDialogManager;
	}

	{
		m_pkUIManager					= T_NEW_D UI_UIManager;
		m_pkUIManager->GameInit();

#   ifndef _MAP_VIEWER_BUILD

		// UI avatar 管理
		{
			m_pkAvatarManager			= T_NEW_D UI_AvatarManager;
			m_pkAvatarManager->Init();

			m_pkUIManager->RegisterAvatarManager(m_pkAvatarManager);
		}

		// UI effect 管理
		{
			T_PROFILE_TIMER("初始化 UI Effect");

			m_pkUiEffectManager         = T_NEW_D UE_UiEffectManager;
		}
#   endif // _MAP_VIEWER_BUILD

		m_pkUIManager->RegisterToLua();
	}

	if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 4, CLIENTAPP_FLASH_TEXT_5 );
	// 渲染
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化渲染管理");
	{
		m_pkCameraManager               = T_NEW_D CA_CameraManager;
		m_pkCameraManager->Init();

		m_pkSceneManager				= T_NEW_D SE_SceneManager;

		m_pkRenderManager->CreateRenderFrame();

		m_pkPointLightManager			= T_NEW_D RG_ClientPointLightManager;
							  
		m_pkCameraAnimManager           = T_NEW_D CA_CameraAnimationManager;

		m_pkCutSceneMgr                 = T_NEW_D UT_CutSceneManager;
		m_pkCutSceneMgr->Init();

		UT_StatisticsManager* pStatisticsManager = NiNew UT_StatisticsManager;
		pStatisticsManager->Init();
	}

	// 物品
	{
		LOG_MEMORY_HELPPER("LC_ClientItemFactory");

		m_pkItemFactory                 = T_NEW_D LC_ClientItemFactory;
		m_pkItemFactory->Init();

		m_pkPackEntryFactory			= T_NEW_D LC_ClientPackEntryFactory;
		m_pkPackEntryFactory->Init();
	}

	// 特效
	{
		T_PROFILE_TIMER("初始化特效系统");

		m_pkFXManager->Init();
		m_pkFXManager->SetCamera(m_pkCameraManager->GetNiCamera());
		m_pkFXManager->OnRenderTargetResized();
	}

	// 输入
	{
		T_PROFILE_TIMER("初始化输入系统");

		m_pkInputManager				= T_NEW_D IP_InputManager;
		m_pkInputManager->Init(m_hWnd,m_hInst);
		m_pkShortcutKeyCodeMgr			= T_NEW_D IP_ShortcutKeyCodeMgr;
	}



	// 场景和角色
	{
		T_PROFILE_TIMER("初始化场景与角色管理器");
		if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 5, CLIENTAPP_FLASH_TEXT_6 );

		m_pkSceneManager->Init();

		m_pkIMManager					= T_NEW_D IM_Manager;
		m_pkIMManager->Init();

		m_pkDynamicCollisionManager		= T_NEW_D GD_DynamicCollisionManager;

		m_pkBackLoadingProcess          = T_NEW_D BL_AsyncOperationManager( m_pkSceneManager->GetWorldMap() );
		m_pkBackLoadingProcess->Init();
		//启动后台加载线程
		//这个时候可能会有UI的加载
		m_pkBackLoadingProcess->StartLoading(0);

		/*
		由于UI会在初始化的时候加载一部分Lua脚本，就会进行资源的创建
		为了统一资源创建为后台加载，这里需要先把UI的后台加载相关的
		消息进行注册
		*/
		//Update
		m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkBackLoadingProcess );
		m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_UI_LOAD_UIIMG,			m_pkBackLoadingProcess );
		m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_UI_UNLOAD_UIIMG,			m_pkBackLoadingProcess );

	}

	
	if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 6, CLIENTAPP_FLASH_TEXT_7 );
	//公会副本数据
	{
		m_pkGuildRaidDataManager		= T_NEW_D LC_GuildRaidDataManager;
	}

	// 客户端商城
	{
		m_pkClientShopCity				= T_NEW_D LC_ClientShopCity;
	}

	// NPC商店
	{
		m_pkNPCShopManager				= T_NEW_D LC_NPCShopManager;
		m_pkNPCShopManager->Init();
	}

	// NPCTalk 和npc纯聊天
	{
		m_pkConversationManager			= T_NEW_D LC_ConversationManager;
		m_pkConversationManager->Init();
	}

	// 地图传送
	{
		m_pkMapTranscriptionManager		= T_NEW_D LC_ClientMapTranscriptionManager;
		m_pkMapTranscriptionManager->Init();
	}

	// 过场动画
	{
		m_pkUICutScenesManager			= T_NEW_D LC_ClientUICutScenesManager;
		m_pkUICutScenesManager->Init();
	}

	// NPC传送
	{
		m_pkNPCTransferManager			= T_NEW_D LC_NPCTransferManager;
		m_pkNPCTransferManager->Init();
	}

	//tooltip
	{
		m_pkTooltipManager				= T_NEW_D LC_TooltipManager;
		m_pkTooltipManager->Init();
	}

	// AudioManager
	{
		T_PROFILE_TIMER("初始化逻辑管理器");

		m_pkSoundManager                = T_NEW_D AS_ClientSoundManager;
		m_pkSoundManager->Init(m_hWnd);
	}

	// 邮件
	{
		m_pkMailManager					= T_NEW_D LC_ClientMailManager;		
	}

	// 拍卖
	{
		m_pkAuctionManager				= T_NEW_D LC_ClientAuctionManager;
	}

	// 门派
	{
		m_pkGuildManager				= T_NEW_D LC_ClientGuildManager;
	}

	// 门派家园
	{
		m_pkGuildHomeMgr				= T_NEW_D LC_ClientGuildHomeMgr;
	}

	//阵营信息
	{
		m_pkFactionManager				= T_NEW_D LC_FactionManager;
		m_pkFactionManager->Init();
	}

	//目标过滤管理器
	{
		m_pkActorFilterManager			= T_NEW_D LC_ActorFilterManager;
		m_pkActorFilterManager->Init();
	}

  /*
	{
		m_pkClientExchangeManager   = T_NEW_D  LC_ClientExchangeManager;
		m_pkClientExchangeManager->Init();
	}
  */

	////游戏币交易系统
	//{
	//	m_pkClientCurrencyTradeManager   = T_NEW_D  LC_ClientCurrencyTradeManager;
	//}

	//称号管理
	{
		m_pkTitleManger = T_NEW_D LC_TitleManager;
		m_pkTitleManger->Init();
	}

	{
		ItemEffectManage * inst = T_NEW_D ItemEffectManage();
		inst->Init();
	}
	{
		SK_SP * obj = T_NEW_D SK_SP;
		obj->Init();
	}
	{
		ItemSuitManager * inst = T_NEW_D ItemSuitManager;
		inst->Init();
	}
	{
		LC_SubordinateManager *inst = T_NEW_D LC_SubordinateManager;
		inst->Init();
	}
	// 必须在所有RegisterToLua调用后调用
	// 如果这里崩溃，说明LuaState的初始化顺序出问题了！
	if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 7, CLIENTAPP_FLASH_TEXT_8 );
	{
		T_PROFILE_TIMER("初始化脚本资源");

		T_ASSERT(LS_LuaState::GetInitFuncListSize() != 0);

		m_pkLuaStateFactory->Init();

		m_pkScriptInterface->Init();

		InitPlatformLuaFunc();
	}
	commitPatchProgress(EVENT_CLIENT_INIT_6, 0);

	//任务
	{
		T_PROFILE_TIMER("初始化任务脚本资源");
		LOG_MEMORY_HELPPER("LC_ClientTaskManager");
		m_pkTaskManager = T_NEW_D LC_ClientTaskManager;
		m_pkTaskManager->Init();

		m_pkLuaStateFactory->GetLogicLuaState()->ClearPropertyTableCache();
	}

	//VIP
	{
		m_pkVIPCmnManager = T_NEW_D LC_VIPCmnManager;
		m_pkVIPCmnManager->Init();
	}
	//AttributeManager
	{
		LC_ActorAttributeManager* pkMgr = T_NEW_D LC_ActorAttributeManager;
		pkMgr->Init();
	}
	if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 8, CLIENTAPP_FLASH_TEXT_9 );
	//竞技场管理器
	{
		m_pkArenaManager	= T_NEW_D LC_ClientArenaManager;
	}  

	{
		m_pkCommonStatManager = T_NEW_D LC_CommonStatManager;
		m_pkCommonStatManager->Init();
	}
	{
		m_pkCommonShopManager = T_NEW_D LC_ClientShopManager;
		m_pkCommonShopManager->Init();
	}

	//{
	//	m_pkSoulRefineManager = T_NEW_D LC_ClientSoulRefineManager;
	//	m_pkSoulRefineManager->Init();
	//}
	//{
	//	m_pkSoulRefineDataAsset = T_NEW_D LC_SoulRefineDataAsset;
	//	m_pkSoulRefineDataAsset->Init();
	//}
	{
		m_pkShotActivityManager = T_NEW_D LC_ClientShotActivityManager;
		m_pkShotActivityManager->Init();
	}
	{
		LOG_MEMORY_HELPPER("m_pkActivityManager");

		m_pkActivityManager = T_NEW_D LC_ClientActivityManager;
		m_pkActivityManager->Init();
	}
	//成就管理器
	{
		m_pkAchievementManager = T_NEW_D LC_AchievementManager;
		m_pkAchievementManager->Init();
	}

	//{
	//	m_pkMeltDataManager = T_NEW_D LC_MeltDataManager;
	//	m_pkMeltDataManager->Init();
	//}
	//{
	//	m_pkBoneSoulManager = T_NEW_D LC_BoneSoulManager;
	//	m_pkBoneSoulManager->Init();
	//}
	//{
	//	m_pkStarsRuneManager = T_NEW_D LC_StarsRuneManager;
	//	m_pkStarsRuneManager->Init();
	//}	
	//{
	//	m_pkMillionWeaponManager = T_NEW_D LC_MillionWeaponManager;
	//	m_pkMillionWeaponManager->Init();
	//}

	{
		m_pkPetManager = T_NEW_D LC_ClientToyPetManager;
		m_pkPetManager->Init();
	}

	{
		m_pkCollectionManager = T_NEW_D LC_ClientCollectionManager;
		m_pkCollectionManager->Init();
	}

	{
		m_pkBotManager = T_NEW_D LC_ClientBotManager;
		m_pkBotManager->init();
	}

	{
		m_pkCombatRecordManager = T_NEW_D LC_ClientCombatRecordManager;
		m_pkCombatRecordManager->Init();
	}


	{
		m_pkAchievementManager_Common = T_NEW_D CAchievementManager;
		m_pkAchievementManager_Common->Init();
	}

	{
		m_pkGuildProfitManager = T_NEW_D LC_ClientGuildProfitManager;
		m_pkGuildProfitManager->Init();
	}
	
	{
		m_pkArenaPlayBackManager = T_NEW_D LC_ClientArenaPlayBackManager;
		m_pkArenaPlayBackManager->Init();
	}

	{
		m_pkReportManager = T_NEW_D LC_ClientReportManager;
		m_pkReportManager->Init();
	}

	// UI
	{
		LOG_MEMORY_HELPPER("load_ui");

		T_PROFILE_TIMER("初始化 UI 资源");
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"init ui system start");
		int nLuaCreation = LS_LuaState::s_total_objects_created;
		int nLuaDestroy = LS_LuaState::s_total_objects_destroyed;
		m_pkUIManager->Init(m_hWnd);
		m_pkUiEffectManager->Init();
		GfxWriteLog( LOG_TAG_STAT , LOG_SWITCH_STAT ,
			"UI Lua Object Creation:%d", 
			LS_LuaState::s_total_objects_created - nLuaCreation );

		GfxWriteLog( LOG_TAG_STAT , LOG_SWITCH_STAT ,
			"UI Lua Object Destruction:%d", 
			LS_LuaState::s_total_objects_destroyed - nLuaDestroy );

		m_pkLuaStateFactory->GetLogicLuaState()->ClearPropertyTableCache();

		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"init ui system success");
	}
	if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 9, CLIENTAPP_FLASH_TEXT_10 );
	commitPatchProgress(EVENT_CLIENT_INIT_7, 0);

	{
		m_pkDebugCmdParser->Init();
	}

#ifndef _MAP_VIEWER_BUILD
	// 技能
	{
		LOG_MEMORY_HELPPER("load_skill_script");
		T_PROFILE_TIMER("初始化技能脚本");
		m_pkSkillFactory                = T_NEW_D SK_Factory;

		int nLuaCreation = LS_LuaState::s_total_objects_created;
		int nLuaDestroy = LS_LuaState::s_total_objects_destroyed;

		m_pkSkillFactory->Init(true);

		GfxWriteLog( LOG_TAG_STAT , LOG_SWITCH_STAT ,
			"Skill Lua Object Creation:%d", 
			LS_LuaState::s_total_objects_created - nLuaCreation );

		GfxWriteLog( LOG_TAG_STAT , LOG_SWITCH_STAT ,
			"Skill Lua Object Destruction:%d", 
			LS_LuaState::s_total_objects_destroyed - nLuaDestroy );

		m_pkLuaStateFactory->GetLogicLuaState()->ClearPropertyTableCache();
	}
#endif
	if ( m_pkSplash )		m_pkSplash->SetSlideProgress( 10, CLIENTAPP_FLASH_TEXT_11 );
	commitPatchProgress(EVENT_CLIENT_INIT_8, 0);

	// 逻辑
	{
		T_PROFILE_TIMER("初始化逻辑管理器");
		LOG_MEMORY_HELPPER("load_logic_manager");

		m_pkLogicManager                = T_NEW_D LC_ClientLogicManager;
		//m_pkLogicManager->Init();
	}
	// ID 管理
	{
		//m_pkLogicIDManager              = T_NEW_D LC_LogicIDManager;
		//m_pkLogicIDManager->Init();
	}

	// 逻辑区域管理
	{
		T_PROFILE_TIMER("初始化逻辑区域管理器");
		LOG_MEMORY_HELPPER("load_logic_area");

		//m_pkLogicMapManager            = T_NEW_D LC_LogicMapManager;
		//m_pkLogicMapManager->Load();
		m_pkLogicMapManager            = T_NEW_D LC_ClientLogicMapManager;
		m_pkLogicMapManager->Load();

		m_pkLogicTriggerManager         = T_NEW_D LC_LogicTriggerManager;
		m_pkLogicTriggerManager->Init();
	}
	commitPatchProgress(EVENT_CLIENT_INIT_9, 0);

	// 子弹
	{
		m_pkBulletManager = T_NEW_D GD_ClientBulletManager;
		m_pkPickMeshManager = T_NEW_D UT_Pick_Manager();
	}

	// NPC交互管理
	{
		m_pkNPCInteractionManager = T_NEW_D LC_ClientNPCInteractionManager;
		m_pkNPCInteractionManager->Init();
	}

	//操作管理
	{
		m_pkClientActorOperationManager = T_NEW_D LC_ClientActorOperationManager;
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "初始化管理器 结束");
	if ( m_pkSplash )		m_pkSplash->SetProgress( 100, L"正在进入游戏..." );
	{
		m_pkTreasureChestsManager = T_NEW_D LC_TreasureChestsManager;
		m_pkTreasureChestsManager->Init();
	}
	{
		m_pkScriptActionManager = T_NEW_D ScriptAction::SA_ActionManager;
		m_pkScriptActionManager->Init("ScriptAction.lua");
	}
	{
		m_pkAutoFitNPCManager = T_NEW_D AutoFitNPCManager;
		m_pkAutoFitNPCManager->Init();
	}

	{
		m_pkWalkTransferManager = T_NEW_D SM_WalkTransferManager;
		m_pkWalkTransferManager->InitMapDis();
	}

	{
		m_pkNoticeAnalysisManager = T_NEW_D LC_NoticeAnalysisMgr;
	}
	
	{
		m_pkConfigReloadManager = T_NEW_D ClientConfigReloadManager;
		m_pkConfigReloadManager->Init();
	}
	{
		m_pkYunYingManager = T_NEW_D LC_YunYingManager;
	}
	return true;
}
//------------------------------------------------------------------------------
bool ClientApp::_uninitManger()
{
#pragma TODO("所有管理器的 UnInit 都应该在 MessageCenter 析构之前先做，然后再做各管理器的析构")
	// 这样才能统一 UnInit 的资源释放流程到后台加载管理器中

	UT_StatisticsManager::Destroy();

	//由于退出时的依赖关系比较复杂
	//所以在退出的时候先中止后台加载线程，再删除SceneManager
	if(m_pkBackLoadingProcess)
	{
		m_pkBackLoadingProcess->StopLoading(true);

		m_pkBackLoadingProcess->UnInit();
		T_DELETE_D m_pkBackLoadingProcess;
		m_pkBackLoadingProcess = NULL;
	}

	T_SAFE_DELETE(m_pkYunYingManager);

	if (m_pkNetManager)
		m_pkNetManager->UnInit();
	T_SAFE_DELETE(m_pkNetManager);

	if (m_pkMessageCenter)
		m_pkMessageCenter->Update(GET_PROCESS_TIME);
	T_SAFE_DELETE( m_pkMessageCenter );

	T_SAFE_DELETE(m_pkGuildRaidDataManager);

	T_SAFE_DELETE(m_pkClientShopCity);

	T_SAFE_DELETE(m_pkAuctionManager);

	T_SAFE_DELETE(m_pkGuildManager);

	T_SAFE_DELETE(m_pkGuildHomeMgr);

	T_SAFE_DELETE(m_pkMailManager);

	T_SAFE_DELETE(m_pkAchievementManager);

	T_SAFE_DELETE(m_pkWalkTransferManager);

	T_SAFE_DELETE(m_pkNoticeAnalysisManager);

	if (m_pkFactionManager)
		m_pkFactionManager->UnInit();
	T_SAFE_DELETE(m_pkFactionManager);

	if (m_pkConfigReloadManager)
		m_pkConfigReloadManager->UnInit();
	T_SAFE_DELETE(m_pkConfigReloadManager);

	if (m_pkSoulRefineManager)
		m_pkSoulRefineManager->UnInit();
	T_SAFE_DELETE(m_pkSoulRefineManager);

	if (m_pkSoulRefineDataAsset)
		m_pkSoulRefineDataAsset->clear();
	T_SAFE_DELETE(m_pkSoulRefineDataAsset);

	if (m_pkShotActivityManager)
	{
		m_pkShotActivityManager->Unit();
	}
	T_SAFE_DELETE(m_pkShotActivityManager);

	if (m_pkActivityManager)
	{
		m_pkActivityManager->Unit();
	}
	T_SAFE_DELETE(m_pkActivityManager);

	if(m_pkMeltDataManager)
	{
		m_pkMeltDataManager->clear();
	}
	T_SAFE_DELETE(m_pkMeltDataManager);

	if(m_pkBoneSoulManager)
	{
		m_pkBoneSoulManager->Clear();
	}
	T_SAFE_DELETE(m_pkBoneSoulManager);

	if ( m_pkStarsRuneManager )
	{
		m_pkStarsRuneManager->Clear();
	}
	T_SAFE_DELETE(m_pkStarsRuneManager);

	if(m_pkMillionWeaponManager)
	{
		m_pkMillionWeaponManager->Unit();
	}
	T_SAFE_DELETE(m_pkMillionWeaponManager);

	T_SAFE_DELETE(m_pkPetManager);

	T_SAFE_DELETE(m_pkCollectionManager);

	T_SAFE_DELETE(m_pkBotManager);
	T_SAFE_DELETE(m_pkCombatRecordManager);
	T_SAFE_DELETE(m_pkAchievementManager_Common);

	if(m_pkGuildProfitManager)
		m_pkGuildProfitManager->Unit();
	T_SAFE_DELETE(m_pkGuildProfitManager);

	if (m_pkReportManager)
		m_pkReportManager->UnInit();
	T_SAFE_DELETE(m_pkReportManager);
	

	if (m_pkActorFilterManager)
		m_pkActorFilterManager->UnInit();
	T_SAFE_DELETE(m_pkActorFilterManager);

	if (m_pkNPCShopManager)
		m_pkNPCShopManager->UnInit();
	T_SAFE_DELETE(m_pkNPCShopManager);

	if (m_pkNPCTransferManager)
		m_pkNPCTransferManager->UnInit();
	T_SAFE_DELETE(m_pkNPCTransferManager);

	if (m_pkTaskManager)
		m_pkTaskManager->UnInit();
	T_SAFE_DELETE(m_pkTaskManager);

	if (m_pkItemFactory)
		m_pkItemFactory->UnInit();
	T_SAFE_DELETE(m_pkItemFactory);

	if (m_pkPackEntryFactory)
		m_pkPackEntryFactory->UnInit();
	T_SAFE_DELETE(m_pkPackEntryFactory);

	if (m_pkVIPCmnManager)
		m_pkVIPCmnManager->UnInit();
	T_SAFE_DELETE(m_pkVIPCmnManager);

	LC_ActorAttributeManager* pkMgr = LC_ActorAttributeManager::GetSingletonPtr();
	if(pkMgr != NULL)
		pkMgr->UnInit();
	T_SAFE_DELETE(pkMgr);

	if (m_pkLogicMapManager)
		m_pkLogicMapManager->Shutdown();
	T_SAFE_DELETE(m_pkLogicMapManager);

	if (m_pkLogicTriggerManager)
		m_pkLogicTriggerManager->Shutdown();
	T_SAFE_DELETE(m_pkLogicTriggerManager);

	//m_pkLogicIDManager->UnInit();
	//T_SAFE_DELETE(m_pkLogicIDManager);

	if (m_pkConversationManager)
		m_pkConversationManager->UnInit();
	T_SAFE_DELETE(m_pkConversationManager);

	if (m_pkMapTranscriptionManager)
		m_pkMapTranscriptionManager->UnInit();
	T_SAFE_DELETE(m_pkMapTranscriptionManager);

	if (m_pkUICutScenesManager)
		m_pkUICutScenesManager->UnInit();
	T_SAFE_DELETE(m_pkUICutScenesManager);

#ifndef _MAP_VIEWER_BUILD
	if (m_pkSkillFactory)
		m_pkSkillFactory->UnInit();
	T_SAFE_DELETE(m_pkSkillFactory);
#endif

	if (m_pkTooltipManager)
		m_pkTooltipManager->UnInit();
	T_SAFE_DELETE(m_pkTooltipManager);

	if (m_pkIMManager)
		m_pkIMManager->UnInit();
	T_SAFE_DELETE(m_pkIMManager);

	// UI相关
	{
		//UE会释放材质所以放到后台线程退出之后进行
#ifndef _MAP_VIEWER_BUILD

		if (m_pkUiEffectManager)
			m_pkUiEffectManager->UnInit();
		T_SAFE_DELETE(m_pkUiEffectManager);

		//由于和ui系统的耦合过紧密，所以需要拆分析构顺序；
		if ( m_pkUIManager )
			m_pkUIManager->UnRegisterAvatarManager();
		if (m_pkAvatarManager)
			m_pkAvatarManager->PreUnInit();

#endif // _MAP_VIEWER_BUILD

		if (m_pkUIManager)
			m_pkUIManager->UnInit();
		T_SAFE_DELETE(m_pkUIManager);


#ifndef _MAP_VIEWER_BUILD

		//由于和ui系统的耦合过紧密，所以需要拆分析构顺序；
		if (m_pkAvatarManager)
			m_pkAvatarManager->UnInit();
		T_SAFE_DELETE(m_pkAvatarManager);

#endif // _MAP_VIEWER_BUILD

	}
	
	T_SAFE_DELETE(m_pkPointLightManager);

	if (m_pkSceneManager)
		m_pkSceneManager->UnInit();
	T_SAFE_DELETE(m_pkSceneManager);

	T_SAFE_DELETE(m_pkAsyncModelManager);

	if (m_pkInputManager)
		m_pkInputManager->UnInit();	
	T_SAFE_DELETE(m_pkInputManager);
	T_SAFE_DELETE(m_pkShortcutKeyCodeMgr);

	if (m_pkFXManager)
		m_pkFXManager->UnInit();
	T_SAFE_DELETE(m_pkFXManager);

	// 客户端需要手动清除这些静态数据
	LC_MapBase::ManualClearHelperData();
	if (m_pkMapMananger)
		m_pkMapMananger->UnInit();
	T_SAFE_DELETE(m_pkMapMananger);
	
	if (m_pkNewBeeManager)
		m_pkNewBeeManager->UnInit();
	T_SAFE_DELETE(m_pkNewBeeManager);

	T_SAFE_DELETE(m_pkDialogManager);

	T_SAFE_DELETE(m_pkTextFilter);

	if (m_pkLogicManager)
		m_pkLogicManager->UnInit();
	T_SAFE_DELETE(m_pkLogicManager);

	T_SAFE_DELETE(m_pkDynamicCollisionManager);

	if (m_pkTextManager)
		m_pkTextManager->UnInit();
	T_SAFE_DELETE(m_pkTextManager);

	if (m_pkDebugCmdParser)
		m_pkDebugCmdParser->UnInit();
	T_SAFE_DELETE(m_pkDebugCmdParser);

	T_SAFE_DELETE(m_pkTimer);
	T_SAFE_DELETE(m_pkTimerManager);

	if (m_pkStateFactory)
		m_pkStateFactory->UnInit();
	T_SAFE_DELETE(m_pkStateFactory);

	if (m_pkControllerFactory)
		m_pkControllerFactory->UnInit();
	T_SAFE_DELETE(m_pkControllerFactory);



	T_SAFE_DELETE(m_pkWorldConfig);

	if (m_pkPathManager)
		m_pkPathManager->UnInit();
	T_SAFE_DELETE(m_pkPathManager);

	if (m_pkScriptInterface)
		m_pkScriptInterface->UnInit();
	T_SAFE_DELETE(m_pkScriptInterface);

	if (m_pkScriptActionManager)
		m_pkScriptActionManager->Unit();
	T_SAFE_DELETE(m_pkScriptActionManager);

	{
		UnInitPlatformLuaFunc();
	}

	if (m_pkLuaStateFactory)
	{
		m_pkLuaStateFactory->UnInit();
	}
	T_SAFE_DELETE(m_pkLuaStateFactory);

	T_SAFE_DELETE(m_pkClassRegistry);

	// 渲染
	{
		if ( m_pkRenderManager )
			m_pkRenderManager->PreUnInit();

		if (m_pkCameraManager)
		{
			m_pkCameraManager->UnInit();
			CA_CameraManager::DestroyInstance();
		}

		CA_CameraAnimationManager::DestroyInstance();

		if (m_pkCutSceneMgr)
		{
			m_pkCutSceneMgr->Shutdown();
			T_SAFE_DELETE(m_pkCutSceneMgr);
		}

		if (m_pkModelContentManager)
			m_pkModelContentManager->UnInit();
		T_SAFE_DELETE(m_pkModelContentManager);

		if (m_pkMaterialManager)
			m_pkMaterialManager->UnInit();
		T_SAFE_DELETE(m_pkMaterialManager);		

		if (m_pkBreakModelManager)
			m_pkBreakModelManager->UnInit();
		T_SAFE_DELETE(m_pkBreakModelManager);
	
		if ( m_pkRenderManager )
			m_pkRenderManager->UnInit();
		RG_RenderManager::DestroyInstance();
	}

	T_SAFE_DELETE(m_pkBulletManager);
	T_SAFE_DELETE(m_pkPickMeshManager);

	if (m_pkDirConfig)
		m_pkDirConfig->UnInit();
	T_SAFE_DELETE(m_pkDirConfig);

	if (m_pkSoundManager)
		m_pkSoundManager->UnInit();
	T_SAFE_DELETE(m_pkSoundManager);

	LC_MasterBlissManager* pkMasterBlissManager = LC_MasterBlissManager::GetSingletonPtr();
	if ( pkMasterBlissManager )
	{
		pkMasterBlissManager->UnInit();
		T_SAFE_DELETE(pkMasterBlissManager);
	}

	LC_SkillBookManager* pkSkillBookManager = LC_SkillBookManager::GetSingletonPtr();
	if ( pkSkillBookManager )
	{
		pkSkillBookManager->Unit();
		T_SAFE_DELETE(pkSkillBookManager);
	}

	T_SAFE_DELETE(m_pkHelperDiffManager);

	UnInit_Client_CSV_File();

	T_SAFE_DELETE( m_pkClientActorOperationManager );

	T_SAFE_DELETE(m_pkProfileManager);

	T_SAFE_DELETE(m_pkLogManager);

	T_SAFE_DELETE(m_pkNPCInteractionManager);

	T_SAFE_DELETE(m_pkArenaManager);

	//T_SAFE_DELETE(m_pkClientExchangeManager);

	//T_SAFE_DELETE(m_pkClientCurrencyTradeManager);

	T_SAFE_DELETE(m_pkTitleManger);

	{
		ItemEffectManage::DestroyInstance();

		SK_SP::DestroyInstance();

		ItemSuitManager::DestroyInstance();
	}
	{
		LC_SubordinateManager *inst = LC_SubordinateManager::GetSingletonPtr();
		if (inst)
			inst->Unit();
		T_SAFE_DELETE(inst);
	}

	if (m_pkSessionManager)
		m_pkSessionManager->UnInit();
	T_SAFE_DELETE(m_pkSessionManager);

	if (m_pkCommonStatManager)
		m_pkCommonStatManager->Unit();
	T_SAFE_DELETE(m_pkCommonStatManager);
	
	if (m_pkTreasureChestsManager)
		m_pkTreasureChestsManager->Unit();
	T_SAFE_DELETE(m_pkTreasureChestsManager);

	if (m_pkCommonShopManager)
		m_pkCommonShopManager->Unit();
	T_SAFE_DELETE(m_pkCommonShopManager);

	if (m_pkAutoFitNPCManager)
		m_pkAutoFitNPCManager->UnInit();
	T_SAFE_DELETE(m_pkAutoFitNPCManager);

	if (m_pkArenaPlayBackManager)
		m_pkArenaPlayBackManager->UnInit();
	T_SAFE_DELETE(m_pkArenaPlayBackManager);

	LM_LocaleManager *pkLocaleMgr = LM_LocaleManager::GetSingletonPtr();
	if(pkLocaleMgr)
	{
		pkLocaleMgr->Unit();
		T_SAFE_DELETE(pkLocaleMgr);
	}

	return true;
}
//------------------------------------------------------------------------------
bool ClientApp::_registerMessageHandler()
{
	// 注册Input消息时需要注意顺序关系，因为会依赖它作消息处理的互斥
	// 按消息类型顺序注册消息，避免同一消息的依赖关系

	//Mouse LButton Pressed
	//m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_LONG_PRESSED, m_pkUIManager);
	//m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_LONG_PRESSED, m_pkLogicManager);

	// Mouse LButton Down
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_DOWN,		m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_DOWN,		m_pkIMManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_DOWN,		m_pkSceneManager);
	// Mouse LButton Up
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_UP,		m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_UP,		m_pkSceneManager);
	// Mouse LButton double click
	//m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_DBCLK,	m_pkIMManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_DBCLK,	m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_LBUTTON_DBCLK,	m_pkSceneManager);
	

	// Mouse RButton Down
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_RBUTTON_DOWN,		m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_RBUTTON_DOWN,		m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_RBUTTON_DOWN,		m_pkIMManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_RBUTTON_DOWN,		m_pkSceneManager);
	// Mouse Wheel
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_WHEEL,			m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_WHEEL,			m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_MBUTTON_DOWN,		m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_MBUTTON_UP,		m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_RBUTTON_UP,		m_pkCameraManager);
	// Mouse Move
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_MOVE,				m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_MOVE,				m_pkCameraManager);	
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_MOVE,				m_pkIMManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_MOUSE_MOVE,				m_pkSceneManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_UP,					m_pkCameraManager);	
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_DOWN,				m_pkCameraManager);	
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_LEFT,				m_pkCameraManager);	
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_RIGHT,				m_pkCameraManager);	
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_COMMA,				m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_PERIOD,				m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_ALT_V,				m_pkCameraManager);	

	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_PAGE_PRIOIR,		m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_PAGE_NEXT,			m_pkCameraManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_ALT,				m_pkUiEffectManager);
#ifdef _MAP_VIEWER_BUILD
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_9,				    m_pkCameraManager);
#endif
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_F9,					m_pkTimer);
	//m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_W,					m_pkTimer);
	
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_1,					m_pkSceneManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_2,					m_pkSceneManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_3,					m_pkSceneManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_1,					m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_2,					m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_3,					m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_9,					m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_ADD,				m_pkLogicManager);

	m_pkMessageCenter->RegisterMessageHandler(MSG_UE_SHOW_PLAYER_HEADALL,		m_pkLogicManager);

//  	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_1,				m_pkSoundManager);
//  	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_2,				m_pkSoundManager);
//  	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_3,				m_pkSoundManager);
//  	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_9,				m_pkSoundManager);

	//m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_ADD,				m_pkRenderManager);
#ifdef ENABLE_CLEAR_MOD_POOL_MANUALLY
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_NUMPAD_MUL,         m_pkAsyncModelManager);
#endif // ENABLE_CLEAR_MOD_POOL_MANUALLY
#ifdef ENABLE_CLEAR_FX_POOL_MANUALLY
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_NUMPAD_MUL,         m_pkFXManager);
#endif // ENABLE_CLEAR_FX_POOL_MANUALLY

	//Timer Start
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_START,					m_pkTimer);
	//Timer Tick
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkInputManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkBulletManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkAsyncModelManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkAvatarManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkSessionManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkSceneManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkFXManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,		            m_pkSoundManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkUiEffectManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkMapMananger);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkTextManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkPathManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkPointLightManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkDynamicCollisionManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkRenderManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkTaskManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkUICutScenesManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkMaterialManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkArenaPlayBackManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkBreakModelManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_WINDOWS_LOST_FOCUS,           m_pkTimer);
	m_pkMessageCenter->RegisterMessageHandler(MGT_WINDOWS_LOST_FOCUS,           m_pkRenderManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_WINDOWS_LOST_FOCUS,           m_pkSoundManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_WIN32_MESSAGE,				m_pkRenderManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_WIN32_MESSAGE,				m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_WIN32_MESSAGE,	            m_pkInputManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_UI_UPDATE,					m_pkUIManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_PLAYER_MOVE,            m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_PLAYER_MOVE,			m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_LCTRL_PRESSED,		m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_LCTRL_RELEASE,		m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_PLAYER_MOVE,			m_pkSoundManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_PLAYER_MOVE,			m_pkPathManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_DEBUG_TERRAIN_CHUNK_LOAD,		m_pkSceneManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_DEBUG_TERRAIN_CHUNK_UNLOAD,	m_pkSceneManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_PLAYER_MOVE,			m_pkSceneManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_PLAYER_MOVE,			m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_CAMERA_RESET,        			m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_CAMERA_ROTATE,        		m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_MODEL_LOADING_TASK,       m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_MODEL_UNLOADING_TASK,     m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_ACTOR_CHANGE_AVATAR_TASK, m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_UIEFFECT_TASK,            m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_UIEFFECT_UNLOADING_TASK,  m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_GFX_TASK,                 m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_GFX_UNLOAD_TASK,          m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_PATH_LOAD_TASK,           m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_PATH_UNLOAD_TASK,         m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_PLAY_VIDEO_LOAD_TASK,     m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_PLAY_AUDIO_LOAD_TASK,     m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_TMAP_LOAD_TASK,			m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_RELOAD_CONFIG_TASK,		m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_TOGGLE_BACKLOADING,           m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_ADD_REPORT_OPERATION_TASK,    m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_CALLBACK_REPORT_OPERATION_TASK,	m_pkReportManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_LBUTTON_PICK_ACTOR,			m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_LBUTTON_PICK_INTERACTIVE_BUTTON,m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_MOUSE_OVER_INTERACTIVE_BUTTON,m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_LBUTTON_DOUBLE_PICK_ACTOR,		m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_RBUTTON_PICK_ACTOR,			m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_MOUSE_OVER_ACTOR,				m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_LBUTTON_PICK_CHEST,			m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_RBUTTON_PICK_CHEST,			m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_MOUSE_OVER_CHEST,				m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_SCENE_L_BUTTON_PICK_TERRAIN,		m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_SCENE_L_BUTTON_DOUBLE_PICK_TERRAIN,m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_SCENE_L_BUTTON_RELEASE_TERRAIN,	m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_SCENE_R_BUTTON_PICK_TERRAIN,		m_pkLogicManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_SCENE_MOUSE_OVER,					m_pkLogicManager);

	if (m_pkUiEffectManager)
	{
		// UE 全部注册给 ui effect manager
		for (int i = (MessageType)MGT_BEGIN_UI_EFFECT_MESSAGE + 1;  i < (MessageType)MGT_END_UI_EFFECT_MESSAGE; ++i)
		{
			m_pkMessageCenter->RegisterMessageHandler((MessageType)i, m_pkUiEffectManager);
		}
		//回复真气
		m_pkMessageCenter->RegisterMessageHandler(MSG_UE_DELTA_SP, m_pkUiEffectManager);
	}

	m_pkMessageCenter->RegisterMessageHandler(MGT_UI_MAP_CHANGE,				m_pkMapMananger);

	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_PLAYER_MOVE,			m_pkMapMananger);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_AREA_SOUND_NAME,		m_pkSoundManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_CAN_SELECT,				m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_CAN_TALK,				m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_CAN_MOVETO,				m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_NOT_MOVETO,				m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_BEG_LOCK_LOCATION,		m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_END_LOCK_LOCATION,		m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_BEG_CAST_SKILL,			m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_LOGIC_END_CAST_SKILL,			m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_INPUT_KEY_F11,				m_pkUIManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_UI_RELOAD_ALL_SCRIPT,			m_pkUIManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_IM_PLAY_ANIMATION,			m_pkIMManager);

	/////Begin CutScenes manager/////////////////////////////////////////////////////////////////////
	m_pkMessageCenter->RegisterMessageHandler(MGT_UI_MAP_CHANGE,				m_pkUICutScenesManager);
	for (int i = (MessageType)MGT_BEGIN_UICUTSCENES_MESSAGE + 1; i < (MessageType)MGT_END_UICUTSCENES_MESSAGE; ++i)
		m_pkMessageCenter->RegisterMessageHandler((MessageType)i, m_pkUICutScenesManager);
	/////End CutScenes manager/////////////////////////////////////////////////////////////////////

#ifdef _MAP_VIEWER_BUILD
	m_pkMessageCenter->RegisterMessageHandler(MGT_UI_MINIMAP_CLICK,             m_pkSceneManager);
#endif

	m_pkMessageCenter->RegisterMessageHandler(MGT_NPC_BORN_BEGIN,				m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_NPC_BORN_BEGIN,				m_pkSoundManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_NPC_BORN_END,					m_pkCameraManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_NPC_DEAD_BEGIN,				m_pkSoundManager);

	//< yun ying platform
	m_pkMessageCenter->RegisterMessageHandler(MGT_YUNYING_ADD_DUOWAN_GET_REPORTABLE,	m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_YUNYING_ADD_DUOWAN_REPORT_CHAT,		m_pkBackLoadingProcess);
	m_pkMessageCenter->RegisterMessageHandler(MGT_YUNYING_ADD_DUOWAN_REPORT_LEVELUP,	m_pkBackLoadingProcess);

	m_pkMessageCenter->RegisterMessageHandler(MGT_TIMER_TICK,					m_pkYunYingManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_YUNYING_NEW_ROLE,				m_pkYunYingManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_YUNYING_PLAYER_LOGIN,			m_pkYunYingManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_YUNYING_PLAYER_LEVELUP,		m_pkYunYingManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_YUNYING_PLAYER_QUIT,			m_pkYunYingManager);
	m_pkMessageCenter->RegisterMessageHandler(MGT_YUNYING_PLAYER_CONSUME,		m_pkYunYingManager);

	m_pkMessageCenter->RegisterMessageHandler(MGT_COMBATRECORD_DAMAGE,			m_pkCombatRecordManager );
	m_pkMessageCenter->RegisterMessageHandler(MGT_COMBATRECORD_HEAL,			m_pkCombatRecordManager );

	return true;
}
//------------------------------------------------------------------------------
bool ClientApp::Start(void)
{
	bool bRet = true;

	bRet |= m_pkSessionManager->Init();

	if (m_pkTimer)
		m_pkTimer->StartTimer();

	return true;
}
//------------------------------------------------------------------------------
bool ClientApp::Run()
{
	m_pkTimer->Tick();

#ifndef _SHIPPING
	float fStartTime = NiGetCurrentTimeInSec();
#endif

	bool bUpdate = m_pkTimer->EnableUpdate();
	if ( bUpdate )
	{
		//////////////////////////////////////////////////////////////////////////

		if (m_pkTimerManager)
		{
			m_pkTimerManager->Tick();
			m_pkTimerManager->Update();
		}
		g_profilingMgr.Reset();

		T_START_PROFILE( "MainLoop" );
		Utility::UT_FileManagerBase::GetSingletonPtr()->Update();
		m_pkMessageCenter->Update(0.0f);
		T_END_PROFILE();


		/////////////////////////////////////////////////////////////////////////

		// 暂时使用这种硬编码的方式来应急一下，
		// 等整合完性能剖析和统计系统之后再来考虑怎么设计。

		{
			bool bSystemMemorySampled = false;
			size_t stSystemWorkingSet = 0, stSystemPeekSize = 0;
			UT_StatisticsManager* pkStats = UT_StatisticsManager::GetSingletonPtr();
			if (pkStats )
			{
				pkStats->SampleFrameRate( m_pkTimer->GetProcessTime() );
				pkStats->CaptureFrameInfo();

				pkStats->SampleMemoryInfo( stSystemWorkingSet , stSystemPeekSize );
				bSystemMemorySampled = true;
			}

			UT_ClientGlobalSettings* pkClientSetting = UT_ClientGlobalSettings::GetSingletonPtr();
			if ( pkClientSetting )
			{
				pkClientSetting->Analyse(m_pkTimer->GetDeltaTime(), 
					stSystemWorkingSet, bSystemMemorySampled);
			}
		}
		
		/////////////////////////////////////////////////////////////////////////
	}
	else
	{
		m_pkLuaStateFactory->Update(m_pkTimer->GetProcessTime(), 0);
		NiSleep(1);
	}

	//////////////////////////////////////////////////////////////////////////

#ifndef _SHIPPING
	static float fCriticalLoopTime = 1.0f;
	float fMainLoopTime = NiGetCurrentTimeInSec() - fStartTime;
	if ( fMainLoopTime > fCriticalLoopTime )
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"[WARNING]main loop time is %f" , fMainLoopTime );
		UT_StatisticsManager* pkStats = UT_StatisticsManager::GetSingletonPtr();
		if (pkStats )
		{
			pkStats->SaveCurrentFrame( fMainLoopTime );
		}
	}
#endif

	return true;
}
//------------------------------------------------------------------------------
bool ClientApp::ProcessWindowsMsg(UINT msg, WPARAM wparam, LPARAM lparam,bool isDectectActiveWindow/* = true*/)
{
	//GfxWriteLog(LOG_TAG_INFO,LOG_SWITCH_INFO,"lPara : %d",lparam);
	if (m_pkMessageCenter)
	{
		if ( isDectectActiveWindow )
		{
			BOOL isVisible = IsWindowVisible( m_hWnd ) ;
			BOOL isIconic = IsIconic( m_hWnd );

			bool bIsWindowActive =( (g_dllRunInBrowser || m_hWnd == GetForegroundWindow()) && 
				IsWindowVisible( m_hWnd ) && !IsIconic( m_hWnd ) );

			static bool bFirst = true;
			if ( m_bWindowsActive != bIsWindowActive || bFirst )
			{
				bFirst = false;
				m_pkMessageCenter->SendMessage( CreateMsg_Windows_Lost_Focus( bIsWindowActive ) , true );
				m_bWindowsActive = bIsWindowActive;

				// 截获PrtScn键为游戏内截屏
				if (m_bWindowsActive)
				{
					RegisterHotKey(m_hWnd, 30903891 , 0, VK_SNAPSHOT);
					RegisterHotKey(m_hWnd, 30903892 , MOD_ALT, VK_SNAPSHOT);
				}
				else
				{
					UnregisterHotKey(m_hWnd, 30903891);
					UnregisterHotKey(m_hWnd, 30903892);
				}

				if (!g_dllRunInBrowser)
				{
					//如果是假全屏，那么只要不是焦点窗口就最小化
					bool bFullScreen = false;
					if ( UT_GlobalSettings::GetSingletonPtr() )
					{
						bFullScreen = g_settings.GetBoolSetting( UT_GlobalSettings::GS_FULLSCREEN );
					}
					if ( bFullScreen  )
					{
						if ( m_bWindowsActive )
						{
							ShowWindow( m_hWnd , SW_MAXIMIZE );
						}
						else
						{
							ShowWindow( m_hWnd , SW_MINIMIZE );
						}
					}
				}
			}
		}
	
		return m_pkMessageCenter->SendMessage(
			CreateMsg_WIN32_MESSGAE(msg, (unsigned int)wparam, (unsigned int)lparam), true);
	}

	return true;
}
void ClientApp::ReqestQuit()
{
	/*if (m_pkMessageCenter)
	{
	m_pkMessageCenter->SendMessage(CreateMsg_LOGIC_SYSTEM_INFO("是否退出游戏",SIT_CONFIRM_QUIT_GAME),true);
	}*/
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	pkInterface->StartCountDown(true);
}


void ClientApp::HideWindow()
{
	if (!g_dllRunInBrowser)
	{
		bool bFullScreen = false;
		if ( UT_GlobalSettings::GetSingletonPtr() )
		{
			bFullScreen = g_settings.GetBoolSetting( UT_GlobalSettings::GS_FULLSCREEN );
		}
		if ( !bFullScreen  )
		{
			ShowWindow( m_hWnd , SW_MINIMIZE );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
UT_LogManager*	 Init_ClientLogManager()
{
//purpose:	管理客户端的LogStream，主要负责产生和销毁LogStream，
//				并将LogStream注册到各个LogTag下
	//默认创建客户端日志文件
	::CreateDirectoryW( L"log" , NULL );

	DWORD pid = ::GetCurrentProcessId();
	wchar_t	strClientLog[MAX_PATH];
	wchar_t	strClientUILog[MAX_PATH];
	swprintf_s(strClientLog, _countof(strClientLog), L"log/Client_%d.log", pid);
	swprintf_s(strClientUILog, _countof(strClientUILog), L"log/Client_UI_%d.log", pid);

	Utility::UT_LogManager   * pkLogManager			= T_NEW_D UT_LogManager;
	Utility::UT_LogStreamFile* pkFileStream			= T_NEW_D UT_LogStreamFile(strClientLog, false);
	Utility::UT_LogStreamFile* pkUIFileStream		= T_NEW_D UT_LogStreamFile(strClientUILog, false);

	//< GameSettings.lua
	mem::vector<int>::iterator iter;
	if(g_settings.FILE_LOG_CONFIG.size() > 0)
	{
		TRAV( g_settings.FILE_LOG_CONFIG , iter )
		{
			pkLogManager->RegisterLog( (LOG_TAG)(*iter) , pkFileStream );
		}
	}

	//打开客户端Critical日志，排查外网Dump问题
	pkLogManager->RegisterLog( LOG_TAG_UI, pkUIFileStream);
	pkLogManager->RegisterLog( LOG_TAG_ERROR, pkUIFileStream);

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "+---------------------------- log system start ----------------------------+");

	StringType exename = GetProcessName();
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, exename.c_str());
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"初始化游戏，CommandLine: %s", ClientCmdlineArg::Get().c_str());

	return pkLogManager;
}
