#include "ClientPCH.h"
#include <Iphlpapi.h>
#include <process.h>
#include "UT_Curl.h"
#include <shellapi.h>


/************************************************************************/
/*                                                                      */
/************************************************************************/


StringType  GetMac(void)
{
	StringType mac("");
	IP_ADAPTER_INFO AdapterInfo[32];       // Allocate information for up to 32 NICs   
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer   
	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo  
		AdapterInfo,                 // [out] buffer to receive data   
		&dwBufLen);                  // [in] size of receive data buffer   
	//No network card? Other error?    
	if(dwStatus != ERROR_SUCCESS)       
		return "00-00-00-00-00-00";    
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;    
	char szBuffer[512]; 
	int count = 0;
	while(pAdapterInfo)    
	{        
		//if(MIB_IF_TYPE_ETHERNET == pAdapterInfo->Type && 71 == pAdapterInfo->Type)
		{	
			if (++count > 8) break;

			sprintf_s(szBuffer, sizeof(szBuffer), "%02X-%02X-%02X-%02X-%02X-%02X;" 
				, pAdapterInfo->Address[0]
			, pAdapterInfo->Address[1]
			, pAdapterInfo->Address[2]
			, pAdapterInfo->Address[3]
			, pAdapterInfo->Address[4]
			, pAdapterInfo->Address[5]); 	
			mac += szBuffer;
		}       
		pAdapterInfo = pAdapterInfo->Next;  
	}	
	return mac;
}

BOOL	RunExe(LPCTSTR exe, WORD iShowFlag, LPCTSTR workdir, LPTSTR cmdline, PROCESS_INFORMATION * ppi, bool bReleaseHandle)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO   si;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.wShowWindow = iShowFlag;  
	si.dwFlags = STARTF_USESHOWWINDOW;

	BOOL bSucceed = FALSE;
	if (::CreateProcess(exe,
		cmdline,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		workdir,
		&si,
		&pi))
	{
		if (ppi != NULL)
			*ppi = pi;
		if (bReleaseHandle)
			::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		bSucceed = TRUE;
	}

	return bSucceed;
}


/************************************************************************/

#define		FTP_UPLOAD_FILE_CMD		"--ftp-create-dirs -T %s -u player:Qqwz1TPaste ftp://123.59.52.87/"

void jzLogToFtp(const char *file, int lineno, const char *category_dir, const char* fmt, ... )
{
	SYSTEMTIME	tm;
	::GetLocalTime(&tm);
	char date[64];
	sprintf(date, "%d.%d.%d.%02d.%02d.%02d", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);

	//////////////////////////////////////////////////////////////////////////
	StringType	uid = ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_LOGIN) + "_" + ClientCmdlineArg::GetValue(CMD_ARG_UID);
	const char *id = uid.c_str();

	//////////////////////////////////////////////////////////////////////////
	const char *slash = strrchr(file, '\\');
	if (!slash)
		slash = file;
	else
		slash++;

	//////////////////////////////////////////////////////////////////////////
	char filename[MAX_PATH];
	sprintf(filename, "log\\%s_%s_%s%d.log", id, date, slash, lineno);
	FILE *fp = fopen(filename, "w");
	if (fp)
	{
		char szText[2048];
		va_list va;
		va_start(va, fmt);
		_vsnprintf(szText,2047,fmt, va);
		va_end(va);

		OutputDebugStringA(__FUNCTION__": ");
		OutputDebugStringA(szText);
		OutputDebugStringA("\n");

		fputs(szText, fp);
		fclose(fp);

		//////////////////////////////////////////////////////////////////////////
		char curl_cmd[2048];
		sprintf(curl_cmd, FTP_UPLOAD_FILE_CMD"%s/%s", filename, category_dir, filename);
		ShellExecute(0, "open", "curl.exe", curl_cmd, NULL, SW_HIDE);
	}

	//wait & delete
	//::DeleteFile(filename);
}

void jzUploadFtpFile( const char *category_dir, const char *file, bool bCopyBeforeUpload, bool bZip )
{
	//< timestamp
	SYSTEMTIME	tm;
	::GetLocalTime(&tm);
	char date[64];
	sprintf(date, "%d.%d.%d.%02d.%02d.%02d", tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);

	//////////////////////////////////////////////////////////////////////////
	StringType	uid = ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_LOGIN) + "_" + ClientCmdlineArg::GetValue(CMD_ARG_UID);

	//////////////////////////////////////////////////////////////////////////
	//< pid in filename
	StringType		pid;
	UT_BaseStringUtility::format(pid, "%d", ::GetCurrentProcessId());
	StringType		file_x = UT_BaseStringUtility::replace(file, "%pid", pid);

	const char *slash = strrchr(file_x.c_str(), '\\');
	if (!slash)
		slash = file_x.c_str();
	else
		slash++;

	//////////////////////////////////////////////////////////////////////////
	StringType local_file = file_x;
	StringType upload_file = uid + "_" + date + "_" + slash;

	OutputDebugStringA(__FUNCTION__": ");
	OutputDebugStringA(upload_file.c_str());
	OutputDebugStringA("\n");

	//< file maybe occupied
	if (bCopyBeforeUpload)
	{
		StringType copy_file = StringType(file_x) + ".copy";
		BOOL bC = ::CopyFile(file_x.c_str(), copy_file.c_str(), FALSE);
		if (bC)
			local_file = copy_file;
	}

	//< file exist?
	if (GetFileAttributes(local_file.c_str()) == INVALID_FILE_ATTRIBUTES)
		return;

	//////////////////////////////////////////////////////////////////////////
	char curl_cmd[2048] = {0};
	if (bZip)
	{
		sprintf(curl_cmd, "%s %s", category_dir, local_file.c_str());	//< category_dir can't be 'trace', see jztr.vcproj
		::ShellExecute(0, "open", "jztr.exe", curl_cmd, NULL, SW_HIDE);
	}
	else
	{
		sprintf(curl_cmd, FTP_UPLOAD_FILE_CMD"%s/%s", local_file.c_str(), category_dir, upload_file.c_str());
		::ShellExecute(0, "open", "curl.exe", curl_cmd, NULL, SW_HIDE);
	}
}

int jzDelOldFile( LPCTSTR filter, long long sec_before )
{
	assert(sec_before > 0);
	int                nDelCount = 0;

	//< search
	WIN32_FIND_DATA find;
	HANDLE hSearch = FindFirstFile(filter, &find);
	if (hSearch != INVALID_HANDLE_VALUE) 
	{
		//< cur time
		SYSTEMTIME        ct;
		FILETIME        ft;
		::GetSystemTime(&ct);        //< 不能用GetLocalTime
		::SystemTimeToFileTime(&ct, &ft);
		LARGE_INTEGER        lft = {ft.dwLowDateTime, ft.dwHighDateTime};

		//< dir
		TCHAR dir[MAX_PATH] = {0};
		_tcsncpy(dir, filter, _countof(dir));
		LPTSTR a = _tcsrchr(dir, _T('\\'));
		LPTSTR b = _tcsrchr(dir, _T('/'));
		LPTSTR c = max(a, b);
		if (c != NULL)
			*(++c) = 0;
		else
			c = dir;

		//< sec to 100ns
		long long _100_ns_before = sec_before * 10000000;

		do 
		{
			//< last write time of file
			LARGE_INTEGER        cft = {find.ftLastWriteTime.dwLowDateTime, find.ftLastWriteTime.dwHighDateTime};

			//< old days
			if (lft.QuadPart - cft.QuadPart >= _100_ns_before)
			{
				_tcsncpy(c, find.cFileName, _countof(dir) - (c - dir) );

				BOOL d = ::DeleteFile(dir);        //< will be deleted if not occupied
				nDelCount += (int)d;
			}
		} 
		while (FindNextFile(hSearch, &find));
		FindClose(hSearch);
	}

	return nDelCount;
}

void BattlePlayBackUpLoad(const char *file, const char *website)
{
	char curl_cmd[2048];
	sprintf(curl_cmd, "-F file=@%s %s", file, website);
	ShellExecute(0, "open", "curl.exe", curl_cmd, NULL, SW_HIDE);
}

void BattlePlayBackDownLoad(const char *file, const char *website)
{
	char curl_cmd[2048];
	sprintf(curl_cmd, "%s -o %s", website, file);
	ShellExecute(0, "open", "curl.exe", curl_cmd, NULL, SW_HIDE);
}

/************************************************************************/

inline char toHex(const BYTE &x)
{
	return x > 9 ? (x - 10 + 'A') : (x + '0');
}

inline char fromHex(const BYTE &x)
{
	return isdigit(x) ? (x - '0') : (x - 'A' + 10);
}

std::string jzUrlEncode( const char * url )
{
	std::string sOut;
	sOut.reserve(strlen(url) * 2);

	char c;
	const char *p = url;
	for( ; c = *p; p++ )
	{
		char buf[4] = {0};
		if( isalnum(c) )
		{      
			buf[0] = c;
		}
		else if ( isspace( c ) ) //貌似把空格编码成%20或者+都可以
		{
			buf[0] = '+';
		}
		else
		{
			buf[0] = '%';
			buf[1] = toHex( (c >> 4) & 0x0f );
			buf[2] = toHex( (c % 16) & 0x0f);
		}
		sOut += (char *)buf;
	}
	return sOut;
}

std::string jzUrlDecode( const char * url )
{
	size_t l = strlen(url);
	if(l == 0)
		return "";

	std::string	temp_buf;
	temp_buf.insert(0, l + 4, ' ');

	int cbDest = 0; //累加
	unsigned char *pSrc = (unsigned char*)url;
	unsigned char *pDest = (unsigned char*)&temp_buf[0];
	while(*pSrc)
	{
		if(*pSrc == '%')
		{
			*pDest = 0;

			//高位
			if (pSrc[1])
			{
				if(pSrc[1] >= 'A' && pSrc[1] <= 'F')
					*pDest += (pSrc[1] - 'A' + 10) * 0x10;
				else if(pSrc[1] >= 'a' && pSrc[1] <= 'f')
					*pDest += (pSrc[1] - 'a' + 10) * 0x10;
				else
					*pDest += (pSrc[1] - '0') * 0x10;
			}
			else
				break;

			//低位
			if (pSrc[2])
			{
				if(pSrc[2] >= 'A' && pSrc[2] <= 'F')
					*pDest += (pSrc[2] - 'A' + 10);
				else if(pSrc[2] >= 'a' && pSrc[2] <= 'f')
					*pDest += (pSrc[2] - 'a' + 10);
				else
					*pDest += (pSrc[2] - '0');
			}
			else
				break;

			pSrc += 3;
		}
		else if(*pSrc == '+')
		{
			*pDest = ' ';
			++pSrc;
		}
		else
		{
			*pDest = *pSrc;
			++pSrc;
		}

		++pDest;
		++cbDest;
	}

	//null-terminator
	*pDest = '\0';
	++cbDest;

	//< 不做utf8 -> unicode -> CP_ACP的处理，默认为全英文，直接返回

	return temp_buf;
}