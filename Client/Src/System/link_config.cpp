#include "ClientPCH.h"


// system
#pragma	comment (lib, "netapi32.lib")
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib, "imm32.lib")
#pragma comment (lib, "Version.lib")
#pragma comment (lib, "Wininet.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "gdiplus.lib")
#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "Shlwapi.lib")
#pragma comment (lib, "psapi.lib")
#pragma comment (lib, "Iphlpapi.lib" )
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "wldap32.lib")

// Gamebryo
#pragma  comment(lib, "NiSystemMT.lib") 
#pragma  comment(lib, "NiMainMT.lib")
#pragma  comment(lib, "NiDX9RendererMT.lib")
#pragma  comment(lib, "NiAnimationMT.lib")
#pragma  comment(lib, "NiCollisionMT.lib")
#pragma  comment(lib, "NiParticleMT.lib")
#pragma  comment(lib, "NiD3DXEffectShaderLibDX9_MT.lib")
#pragma  comment(lib, "NiMilesAudioMT.lib")
#pragma  comment(lib, "NiInputMT.lib")
#pragma  comment(lib, "NiCursorMT.lib")
// miles
#pragma  comment(lib, "mss32.lib")
#pragma  comment(lib, "mss32midi.lib")
// dependency
#pragma comment (lib, "pcre.lib")

//////////////////////////////////////////////////////////////////////////

#define LIB_TYPE		"lib"	// lib, dll
#define LIB_PLATFORM	"x86"	// x86, x64
#define LIB_CODE_TYPE	"mt"	// mt, md

#	if (_MSC_VER == 1400)		// VC80, vs2005
#       define 	LIB_MSCVER "v80"
#	elif (_MSC_VER == 1500)		// VC90, vs2008
#       define 	LIB_MSCVER "v90"
#	elif (_MSC_VER == 1600)		// VC100, vs2010
#       define 	LIB_MSCVER "v100"
#	elif (_MSC_VER == 1700)		// VC110, vs2012
#       define 	LIB_MSCVER "v110"
#	elif (_MSC_VER == 1800)		// VC120, vs2013
#       define 	LIB_MSCVER "v120_xp"
#	elif (_MSC_VER == 1900)		// VC140, vs2015
#       define 	LIB_MSCVER "v140_xp"
#	elif (_MSC_VER == 1910)		// VC150, vs2017
#       define 	LIB_MSCVER "v150_xp"
#   else
#       error "unsupported msvc"
#endif

#ifdef _RELEASE
#       define 	LIB_POSTFIX "r"
#elif _SHIPPING
#       define 	LIB_POSTFIX "s"
#endif

#define LIBNAME_F(n)	n "_" LIB_PLATFORM "_" LIB_MSCVER "_" LIB_CODE_TYPE LIB_POSTFIX ".lib"
#define LIBNAME_R(n)	n "_" LIB_PLATFORM "_" LIB_MSCVER "_" LIB_CODE_TYPE "r" ".lib"
#define LIBNAME_D(n)	n "_" LIB_PLATFORM "_" LIB_MSCVER "_" LIB_CODE_TYPE "d" ".lib"

//////////////////////////////////////////////////////////////////////////

// client
#pragma  comment(lib, LIBNAME_F("Engine_client"))
#pragma  comment(lib, LIBNAME_F("GameLogic_client"))
#pragma  comment(lib, LIBNAME_F("Network_client"))
#pragma  comment(lib, LIBNAME_F("Protocol_client"))
#pragma  comment(lib, LIBNAME_F("UISystem_client"))
#pragma  comment(lib, LIBNAME_F("Utility_client"))

// dependency & tools
#if (_MSC_VER == 1400)		// VC80, vs2005
#	pragma  comment(lib, "DbgLib_MT.lib")
#	pragma  comment(lib, "tinyxml_r_MT.lib")
#	pragma  comment(lib, "freetype241MT.lib")
#	pragma  comment(lib, "zlib_MT.lib")
#	pragma  comment(lib, "Lzma_V8_MT.lib")
#	pragma  comment(lib, "lua_v8_MT.lib")
#	pragma  comment(lib, "libprotobuf-lite_v80_mt.lib")
#	pragma  comment(lib, "winvideo_R_MT.lib")
#	pragma  comment(lib, "dui_flash.lib")
#   pragma  comment(lib, "Downloader_v8_mt_r.lib")
#	pragma  comment(lib, "libcurl_v8_mt.lib")
#else
#	pragma  comment(lib, LIBNAME_R("DbgLib"))
#	pragma  comment(lib, LIBNAME_R("tinyxml"))
#	pragma  comment(lib, LIBNAME_R("freetype281"))
#	pragma  comment(lib, LIBNAME_R("zlib"))
#	pragma  comment(lib, LIBNAME_R("Lzma"))

#ifdef _SHIPPING
#	pragma  comment(lib, LIBNAME_R("lua"))
#else
#	pragma  comment(lib, LIBNAME_D("lua"))
#endif

#	pragma  comment(lib, LIBNAME_R("libprotobuf-lite"))
#	pragma  comment(lib, LIBNAME_R("WinVideo"))
#	pragma  comment(lib, LIBNAME_R("dui_flash"))
#	pragma  comment(lib, LIBNAME_R("Downloader"))
#	pragma  comment(lib, LIBNAME_R("libcurl"))
#endif

//////////////////////////////////////////////////////////////////////////

#undef LIB_TYPE
#undef LIB_PLATFORM
#undef LIB_CODE_TYPE
#undef LIB_MSCVER
#undef LIB_POSTFIX