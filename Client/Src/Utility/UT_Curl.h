/********************************************************************
	created:	2015/03/31
	filename: 	UT_Curl.h
	file path:	Src
	file base:	UT_Curl
	file ext:	h
	author:		ayu
	
	purpose:	
*********************************************************************/

#pragma once

#include <Windows.h>
#include <string>
#include "NewUserEvent.h"

StringType  GetMac(void);
BOOL		RunExe(LPCTSTR exe, WORD iShowFlag, LPCTSTR workdir, LPTSTR cmdline, PROCESS_INFORMATION * ppi = NULL, bool bReleaseHandle = true);

//////////////////////////////////////////////////////////////////////////

//上报运行时错误
void		jzLogToFtp(const char *file, int lineno, const char *category_dir, const char* fmt, ... );
void		jzUploadFtpFile(const char *category_dir, const char *file, bool bCopyBeforeUpload = false, bool bZip = false);
int			jzDelOldFile( LPCTSTR filter, long long sec_before );

void		BattlePlayBackUpLoad(const char *file, const char *website);
void		BattlePlayBackDownLoad(const char *file, const char *website);

/************************************************************************/

std::string		jzUrlEncode( const char * url);
std::string		jzUrlDecode( const char * url);
