//#define _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#pragma comment(lib,"MSVCRTD.LIB")

#include "GameServerApp.h"
#include "UT_ServerCommandParser.h"
#include "TT_TextManager.h"

#ifdef WIN32
#	include "Dbglib.h"
#	include "conio.h"
#	include <tchar.h>
#	include "XDebugDumper.h"
#else
#	include <fpu_control.h>
#	include <csignal>
#	include "coredump.h"
#endif

#include "MG_PACKET.h"
#include <sstream>
#include "call_stack.h"
#include "UT_Log.h"
#include "SystemDefine.h"
#include "char_data.pb.h"
#include "iostream_color.h"
#include "UT_GameServerLogManager.h"
#include "UT_Timer.h"
#include "GlobalSettings.h"
#if T_MEMORY_TRACKER || T_MEMORY_TRACKER_DETOUR
#include "MM_Config.h"
#include "UT_MemTracker.h"
#include "UT_MemHook.h"
//< 早于所有全局变量初始化的全局变量，以便完成初始化工作
#pragma init_seg(compiler)
static Memory::UT_MemHook __mh_initialize_dummy;
#endif
#include "oss_define.h"
#include "UT_Func.h"
#include "UT_TimeUtil.h"
#include "game_version.h"
#include <process.h>
#include <stdio.h>
#ifdef USE_GTEST
	#include <gtest/gtest.h>
#endif

#define SIGKILL 9

using namespace Protocol;

bool bExitApp = false;
bool bReload = false;
#if 0
void _GetExceptionPointers(DWORD dwExceptionCode, 
						  EXCEPTION_POINTERS** ppExceptionPointers)
{
	// The following code was taken from VC++ 8.0 CRT (invarg.c: line 104)

	EXCEPTION_RECORD ExceptionRecord;
	CONTEXT ContextRecord;
	memset(&ContextRecord, 0, sizeof(CONTEXT));

#ifdef _X86_

	__asm {
		mov dword ptr [ContextRecord.Eax], eax
			mov dword ptr [ContextRecord.Ecx], ecx
			mov dword ptr [ContextRecord.Edx], edx
			mov dword ptr [ContextRecord.Ebx], ebx
			mov dword ptr [ContextRecord.Esi], esi
			mov dword ptr [ContextRecord.Edi], edi
			mov word ptr [ContextRecord.SegSs], ss
			mov word ptr [ContextRecord.SegCs], cs
			mov word ptr [ContextRecord.SegDs], ds
			mov word ptr [ContextRecord.SegEs], es
			mov word ptr [ContextRecord.SegFs], fs
			mov word ptr [ContextRecord.SegGs], gs
			pushfd
			pop [ContextRecord.EFlags]
	}

	ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable:4311)
	ContextRecord.Eip = (ULONG)_ReturnAddress();
	ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
	ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);


#elif defined (_IA64_) || defined (_AMD64_)

	/* Need to fill up the Context in IA64 and AMD64. */
	RtlCaptureContext(&ContextRecord);

#else  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif  /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

	ExceptionRecord.ExceptionCode = dwExceptionCode;
	ExceptionRecord.ExceptionAddress = _ReturnAddress();


	EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
	memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
	CONTEXT* pContextRecord = new CONTEXT;
	memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

	*ppExceptionPointers = new EXCEPTION_POINTERS;
	(*ppExceptionPointers)->ExceptionRecord = pExceptionRecord;
	(*ppExceptionPointers)->ContextRecord = pContextRecord;  
}
#endif
#ifdef WIN32
//------------------------------------------------------
DWORD   ThreadFunc(LPVOID   lpVoid)
{
	while (true)
	{
		std::string s;
		getline(std::cin, s, '\n');
		if (0 == strcmp(s.c_str(), "exit"))
		{
			bExitApp = true;
		}
		else
		{
			StringType _cmd(s.c_str());
			TStringVector kVec = UT_BaseStringUtility::SplitString(_cmd, ' ');
			if (kVec.size() >= 2 && 0 == strcmp(kVec[0].c_str(), "acc"))
			{
				Utility::UT_TimerManager::GetSingletonPtr()->Accler(atoi(kVec[1].c_str()));
			}
#ifdef WIN32
			else if (kVec.size() >= 1 && 0 == strcmp(kVec[0].c_str(), "gm"))
			{
				Utility::UT_CommandLine  cmdLine;

				StringStreamType __ss_gm;
				for (size_t i=1; i<kVec.size(); ++i)
				{
					__ss_gm << kVec[i] << " ";
				}
				cmdLine.SplitLine(__ss_gm.str());
				Utility::DebugCommandResult kResult = Utility::LC_CommandParserBase::GetSingletonPtr()->ParseCommand(cmdLine, 0);
				oss_role_gm_command(NULL, 0, cmdLine.GetCommandName(), __ss_gm.str(), kResult.uiRslt);

			}
#endif
			else if(kVec.size() >= 3 && 0 == strcmp(kVec[0].c_str(), "test"))
			{
				bool ret = LC_Helper::CheckIsSameWeek(atoll(kVec[1].c_str()), atoll(kVec[2].c_str()));
				GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, kVec[1], kVec[2], ret);
			}
			else
			{
				TT_TextManager* pkTextManager = TT_TextManager::GetSingletonPtr();
				if (pkTextManager)
				{
					pkTextManager->AddServerAnnoceMsg(s.c_str());
				}
			}
		}
	}
	return 0;
}
//------------------------------------------------------
void   CreateBackThread()
{
	DWORD dwThreadID;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, (LPVOID)NULL, 0, &dwThreadID);
}
#else
void   CreateBackThread()
{
}
#endif

#ifdef WIN32
void LogDebug(const char* info)
{
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "%s", info);
}

void OnException(void* data)
{
	DbgLib::CDebugFx::SetExceptionHandler(false);
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "debug:");
	get_call_stack(LogDebug);
	UT_GameServerLogManager* pkLogManager = UT_GameServerLogManager::GetSingletonPtr();
	pkLogManager->Flush();
	OpenSTDStream();
	std::cout << std::io_color::red << "出错啦，看看有什么问题，按任意键退出。" << std::endl;
	Sleep(2000);
	exit(1);
}
#endif

#if defined(DEBUG) || defined(_DEBUG) || defined(_DEBUG_)
#define BUILD_CONFIG_STRING "Debug"
#else
#define BUILD_CONFIG_STRING "Release"
#endif


class CFrameDelayer
{
public:
	CFrameDelayer()
	{
		m_dwFeedback               = 0;
		m_dwOverwork               = 0;
		m_dwFrameInMillionSeconds  = MILLION_RESOLUTION / FRAME_PER_SECOND;
	}

	~CFrameDelayer()
	{
		m_dwFrameInMillionSeconds  = 0;
		m_dwFeedback               = 0;
		m_dwOverwork               = 0;
	}

	inline void FrameBegin(void)
	{
		m_dwBegin = GET_INTEGER_TIME32;
	}

	inline void FrameEnd(void)
	{
		m_dwEnd      = GET_INTEGER_TIME32;
		m_dwFeedback = m_dwEnd - m_dwBegin;
		DelayFrame();
	}

private:
	void DelayFrame(void)
	{
		// 未使用完的帧时间 >= 0ms
		if (m_dwFrameInMillionSeconds <= m_dwFeedback)
		{
			// 上次执行的时间表明已经超载,记录超载次数,继续执行
			//if (m_dwOverwork++ >= OVERWORK_THRESHOLD)
			{
				// 表明系统已经超载，但是不能让逻辑线程完全占满
				//Sleep(1);// 尝试让出CPU,如果没有>=优先级的就绪线程
				//Sleep(DELTA_MAX);
				// 可能会继续执行剩余时间片
			}
#ifdef WIN32
			Sleep(CPU_TIME_SLICE);
#else
			sleep(CPU_TIME_SLICE);
#endif
		}
		else
		{
			// 未超载,减少超载次数
			if (m_dwOverwork > 0)
			{
				--m_dwOverwork;
			}

			unsigned int dwDelta = m_dwFrameInMillionSeconds - m_dwFeedback;
			// 最少睡眠一个线程时间片
			if (dwDelta < CPU_TIME_SLICE)
			{
				dwDelta = CPU_TIME_SLICE; // 非零值即可最少睡眠一个时间片
			}// 不能超过睡眠上限
			else if (dwDelta > DELTA_MAX)
			{
				dwDelta = DELTA_MAX;
			}
#ifdef WIN32
			Sleep(dwDelta); // 睡眠上轮节省的帧时间
#else
			sleep(dwDelta);
#endif
			//Sleep(DELTA_MAX);
		}
	}

private:
	unsigned int m_dwFrameInMillionSeconds; // 每帧消耗(ms)
	unsigned int m_dwFeedback;              // 上一帧消耗(ms)
	unsigned int m_dwBegin;                 // 帧开始的时间戳(ms)
	unsigned int m_dwEnd;                   // 帧结束的时间戳(ms)
	unsigned int m_dwOverwork;              // 超载次数

private:
    static const unsigned int CPU_TIME_SLICE     = 5;   // CPU时间片,根据不同系统应该有不同的值,通常为10ms~20ms
	static const unsigned int OVERWORK_THRESHOLD = 5;    // 超载阈值
    static const unsigned int FRAME_PER_SECOND   = 20;  // 帧速率
	static const unsigned int MILLION_RESOLUTION = 1000; // 1000ms = 1s
    static const unsigned int DELTA_MAX          = (MILLION_RESOLUTION / FRAME_PER_SECOND);  // 帧睡眠上限
};

#ifdef WIN32
StringType	GetServerConfigPath(int argc, char** argv, const char* pKey, const char* pDefault = NULL, bool bNoPostfix = false)
#else
StringType	GetServerConfigPath(int argc, const char** argv, const char* pKey, const char* pDefault = NULL)
#endif
{
	StringType configPath = "";
	if (pDefault != NULL)
	{
		configPath = pDefault;
	}
	if (argc <= 0 || argv == NULL || pKey == NULL)
	{
		return configPath;
	}

	for (int i = 1; i < argc;)
	{
		if (0 == strcmp(argv[i], pKey))
		{
			configPath = argv[++i];
			break;
		}
		++i;
	}
	if (configPath.length() > 0)
	{
		const char lastKey = configPath[configPath.length() - 1];
		if (lastKey != '/' && lastKey != '\\' && !bNoPostfix)
		{
			configPath = configPath + "/";
		}
	}
	return configPath;
}

#ifdef WIN32

BOOL WINAPI win_signal( DWORD nSignal )
{
	switch ( nSignal )
	{
	case CTRL_C_EVENT:
		printf( "CTRL_C_EVENT\n" );
		break;
	case CTRL_BREAK_EVENT:
		printf( "CTRL_BREAK_EVENT\n" );
		break;
	case CTRL_CLOSE_EVENT:
		printf( "CTRL_CLOSE_EVENT\n" );
		break;
	}
	Sleep(5000);
	printf( "win_signal: %d\n", nSignal );
	return true;
}

static void _crt_handler()
{
	float fNow = GET_PROCESS_TIME;
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "crt error: _crt_handler.<%f>", fNow);
	return;
}

static void _invalid_param_handler(const wchar_t* pcszExpression, const wchar_t* pcszFunction,
							const wchar_t* pcszFile, unsigned int dwLine, uintptr_t reserved)
{
	// TODO: write dump file.
	//INT3();
	//struct _EXCEPTION_POINTERS exp;
	//ExceptionFilter(&exp);
	MessageBox( NULL, "crt error", "crt error, process crash", MB_OK|MB_ICONERROR );
	EXCEPTION_POINTERS* pExcInfo = NULL;
	_GetExceptionPointers( 0, &pExcInfo );
	gstrPrefix = "invalid_param_";
	ExceptionFilter( pExcInfo);
	gstrPrefix = "";
	float fNow = GET_PROCESS_TIME;
	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "crt error: _invalid_param_handler.<%f>", fNow);
	exit(0);
	return;
}
#endif

static void signal_handler(int32_t signal_number)
{
	static bool do_once = true;
	if (do_once == false)
	{
		return;
	}
	do_once = false;
	//saveBackTrace(signal_number, "gamesvrd");
	signal(signal_number, SIG_DFL);
	abort();
	//exit(0);
}

void GameAPP_Update_SEH_shell(GameServerApp* pGame)
{
//#ifdef WIN32
#if 0
	__try
	{
		pGame->Update();
	}
	__except(CrashHandler(GetExceptionInformation()))
	{
	}
#else
	pGame->Update();
#endif
}


#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
static void signal_handler_skip(int32_t signal_number)
{
	return;
}

static void signal_handler(int32_t signal_number)
{
	static bool do_once = true;
	if (do_once == false)
	{
		return;
	}
	do_once = false;
	saveBackTrace(signal_number, "gamesvrd");
	signal(signal_number, SIG_DFL);
	abort();
	//exit(0);
}

static void signal_save_handler(int32_t signal_number)
{
	bExitApp = true;
}

static void signal_handler_reload(int32_t signal_number)
{
	bReload = true;
}

int main(int argc, const char* argv[])
#endif
#ifndef USE_GTEST
	{
		UT_MathBase::GenRandFloat01Cache();
		srand((uint32_t)::time(0));
		//time_t nTimeStamp = LC_Helper::DateTimeToTimeStamp(szTest);
		if (argc > 1)
		{
			//支持版本信息查询
			if (0 == strcmp((const char*)argv[1], "-v"))
			{
				printf("GameServer Version %s\n", GET_CUR_VERSION_INFO());
				return 0;
			}
		}
		//设置异常处理
#ifdef WIN32
		//DbgLib::CDebugFx::SetExceptionCallback(OnException, NULL);
		//DbgLib::CDebugFx::SetExceptionHandler(false);
		MAKE_CORE_DUMP;
		_set_purecall_handler(_crt_handler);
		set_terminate(_crt_handler);
		set_unexpected(_crt_handler);
		_set_invalid_parameter_handler(_invalid_param_handler);
		SetConsoleCtrlHandler(win_signal, true);
#else
		signal(SIGHUP, signal_handler_skip);
		signal(SIGPIPE, signal_handler_skip);
		signal(SIGXFSZ, signal_handler);
		signal(SIGINT, signal_save_handler);
		//signal(SIGTERM, signal_handler);
		signal(SIGABRT, signal_handler);
		signal(SIGILL, signal_handler);
		signal(SIGSEGV, signal_handler);
		signal(SIGFPE, signal_handler);
		signal(SIGUSR1, signal_handler_reload);
		//signal(SIGBREAK, signal_handler);
#endif

#define _FP_EXCEPTION
#ifdef	_FP_EXCEPTION	//开启浮点数异常
	#ifdef WIN32
		unsigned int fpctrl = _controlfp(0, 0);
		fpctrl &= ~(_EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID);
		_controlfp(fpctrl, ~0);
	#else
		fpu_control_t cw;
		_FPU_GETCW(cw);
		cw &= ~_FPU_EXTENDED;
		cw |= _FPU_DOUBLE;
		_FPU_SETCW(cw);
	#endif
#endif

#if T_MEMORY_TRACKER || T_MEMORY_TRACKER_DETOUR
		Memory::UT_MemTracker* pkTracker = new Memory::UT_MemTracker(65536, 65536);
		pkTracker->EnableTrack(false);
#endif
		std::cout << GET_CUR_VERSION_INFO() << std::endl;

		StringType strConfigPath = GetServerConfigPath(argc, argv, "-c", "./Data/Config/");
		StringType strLogPath = GetServerConfigPath(argc, argv, "-l");

		StringType strNode = GetServerConfigPath(argc, argv, "-N", "-1", true);

		StringType strBotOP = GetServerConfigPath(argc, argv, "--Bot", "-1", true);
		int32_t nBotOPCode = boost::lexical_cast<time_t>(strBotOP);

		GameServerApp* pkGameServerApp = T_NEW_D GameServerApp;
		pkGameServerApp->SetConfigPath(strConfigPath.c_str());

		int32_t nServerIdFromArgv = boost::lexical_cast<time_t>(strNode.c_str());
		pkGameServerApp->GetServerManager()->SetServerIdFromArgv(nServerIdFromArgv);

		if (strLogPath.length() > 0)
		{
			pkGameServerApp->SetLogPath(strLogPath.c_str());
		}

		Utility::GlobalSettings* pkSettings = T_NEW_D GlobalSettings;
		if(!pkSettings->Load())
		{
			return 0;
		}

		if (!pkGameServerApp->Init())
		{
			std::cout << std::io_color::green << Utility::Utf8Show("server init error!") << std::endl;
			std::cout << std::io_color::white;
#if !(defined _SHIPPING)
			system("pause");
#else
			Sleep(10000);
#endif
			exit(1);
		}
		CreateBackThread();

#ifdef WIN32
		timeBeginPeriod(1);
#endif
		CFrameDelayer oFrameDelayer;
		pkGameServerApp->SetBotOPCode(nBotOPCode);

		if ( !pkGameServerApp->GetToolMode() )
			std::cout << std::io_color::green << Utility::Utf8Show("GameServer Startup Successfully.") << std::endl;
		std::cout << std::io_color::white;

		CloseSTDStream();

		//穿件pid文件
		char sPidTxt[24] = {'\0'};
		int nId = nServerIdFromArgv;
		if(nId == -1)
			nId = 0;
		sprintf_s(sPidTxt, 24, "%s%d%s","\.\\GamePID_", nId, "\.txt");
		FILE *fp = fopen(sPidTxt,"w");
	   
		if(fp){
			int iPid = (int)getpid();
			fprintf(fp,"%d\n",iPid);
			//关闭流
			fclose(fp);
		}

		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "GameServer Startup Successfully.");

		while (!bExitApp)
		{
			oFrameDelayer.FrameBegin();
			if(bReload == true)
			{
				pkGameServerApp->ReloadCsv("", "");
				bReload = false;
			}
			//pkGameServerApp->Update();
			GameAPP_Update_SEH_shell( pkGameServerApp );
			oFrameDelayer.FrameEnd();
			if ( pkGameServerApp->GetExitByToolMode() )
				bExitApp = true;
		}

		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Server is terminated...", bExitApp);
		if (bExitApp == true)
		{
			pkGameServerApp->Save2Exit();
		}
		pkGameServerApp->StopGameLogic();

		pkGameServerApp->UnInit();
		T_SAFE_DELETE(pkGameServerApp);

#if T_MEMORY_TRACKER
		delete pkTracker;
#endif

#ifdef WIN32
		timeEndPeriod(1);
#endif

		return 0;
	}
#else
	{
		testing::InitGoogleTest(&argc, argv);

		StringType strConfigPath = GetServerConfigPath(argc, argv, "-c", "./Data/Config/");
		StringType strLogPath = GetServerConfigPath(argc, argv, "-l");

		GameServerApp* pkGameServerApp = T_NEW_D GameServerApp;
		pkGameServerApp->SetConfigPath(strConfigPath.c_str());

		if (strLogPath.length() > 0)
		{
			pkGameServerApp->SetLogPath(strLogPath.c_str());
		}
		if (!pkGameServerApp->InitLess())
		{
			exit(1);
		}

		return RUN_ALL_TESTS();
	}
#endif

