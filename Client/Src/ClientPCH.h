#pragma once

// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#define _WIN32_WINNT 0x0501

//#define _STLP_USE_STATIC_LIB

// diable Gamebryo warning
// Disable warning C4251.  Template classes cannot be exported for the obvious
// reason that the code is not generated until an instance of the class is
// declared.  With this warning enabled, you get thousands of complaints about
// class data members that are of template type.  For example, a member such
// as 'NiTPrimitiveArray<NiAVObject*> m_array' generates the warning.
//
// When maintaining the DLL code itself, you might want to enable the warning
// to check for cases where you might have failed to put the NIMAIN_ENTRY after
// the class keyword.  In particular, nested classes must have NIMAIN_ENTRY.
// Also, friend functions must be tagged with NIMAIN_ENTRY.
#pragma warning(disable : 4251)
#pragma warning(disable : 4267)
#pragma warning(disable : 4819)
#pragma warning(disable : 4793)

#pragma warning(disable:4100)   //unreferenced formal parameter
#pragma warning(disable:4201)   //nonstandard extension used : nameless struct/union
#pragma warning(disable:4505)   //unreferenced local function has been removed
#pragma warning( disable : 4251 )
#pragma warning( disable : 4267)
#pragma warning( disable : 4819)
#pragma warning( disable : 4996)
#pragma warning( disable : 4311)//pointer truncation from 'GUISys::CUIWidget *const ' to 'unsigned int'
#pragma warning( disable : 4312) //conversion from 'unsigned int' to 'GUISys::CUIWidget *' of greater size
#pragma warning( disable : 4995)//'wsprintfA'name was marked 



#include "SystemInclude.h"
#include "SystemDefine.h"
#include "StringDefine.h"
#include "NetSessionType.h"

// Protocol Include
#include "MG_PACKET.h"
#include "MG_Common.h"
#include "MG_LoginProtocol.h"
#include "MG_GameProtocol.h"
#include "MG_SkillProtocol.h"
#include "MG_SyncProtocol.h"
#include "MG_ItemProtocol.h"
#include "MG_TaskProtocol.h"
#include "MG_Login2Client.h"
#include "MG_GroupProtocol.h"
#include "MG_DebugProtocol.h"
#include "MG_DuelProtocol.h"
#include "MG_ChatProtocol.h"
#include "MG_Forbid.h"
#include "MG_VerifyProtocol.h"
#include "MG_Profile.h"
//#include "MG_CardProtocol.h"
#include "MG_GiftProtocol.h"
#include "MG_ArenaProtocol.h"
#include "MG_CS_Guild.h"
#include "MG_CS_GuildHome.h"
#include "MG_TeamPlatformProtocol.h"
#include "MG_AssistProtocol.h"
#include "MG_CS_PromoteProtocol.h"

#include "UT_Header.h"

#include "NW_Header.h"

#include "LC_Header.h"

#include "EG_Header.h"

#include "../Build/resource.h"
#include "ClientDefine.h"
#include "ClientResDefine.h"
#include "ClientTextDefine.h"
#include "MultiLang.h"
#include "NewUserEvent.h"
#include "YunYingPlatform.h"
#include "GuildCommDef.h"
#include "ClientCmdlineArg.h"
#include "LC_ClientGamePlayer.h"

#ifndef _MAP_VIEWER_BUILD
// 	#define _ENABLE_PHYSX
#endif

//////////////////////////////////////////////////////////////////////////

void jzLogToFtp(const char *file, int lineno, const char *category_dir, const char* fmt, ... );

//////////////////////////////////////////////////////////////////////////

extern HMODULE			g_hClientModule;
extern HWND				g_hMainWnd;
extern BOOL				g_bMainWndUnicode;
extern int				g_dllRunInBrowser;
extern void	commitPatchProgress(int event, int code);

//////////////////////////////////////////////////////////////////////////

#define		WM_APP_ENTER_GAME_SCENE	(WM_APP + 0x222)			//进入场景，Launcher显示关闭按钮，tx微端特殊要求

#define		WM_INIT_COMPLETE	(WM_APP + 1)					//异步初始化结束
#define		WM_INIT_ON_RESOLUTION_CHANGE (WM_APP + 0x4567)		//INIT结束重新配置分辨率相关(解决最小化后设置错误问题)

//////////////////////////////////////////////////////////////////////////

