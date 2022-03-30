#include "UT_OperateLog.h"
#include <locale.h>
#include "UT_Func.h"
#include "UT_Convert.h"
#include "UT_Macro.h"
#include "game_version.h"
#include "LC_PackStruct.h"
#include "GameServerApp.h"
#include "LC_ServerPlayer.h"
#include "SystemDefine.h"
using namespace OPERATELOG;


/************************************************************************/
extern BOOL MakeDir(const wchar_t * pwDir);

//////////////////////////////////////////////////////////////////////////
//输出到OSS文件的log
//////////////////////////////////////////////////////////////////////////

UT_OperateLogStreamFileOSS::UT_OperateLogStreamFileOSS(const wchar_t * pwPathName,const wchar_t * pwLogDir,int serverid)
: m_strPathName(pwPathName)
{
	//创建目录
	MakeDirectory(pwLogDir);
	m_serverId = serverid;
}

UT_OperateLogStreamFileOSS::~UT_OperateLogStreamFileOSS()
{
	if(m_kFStream)
	{
		fclose(m_kFStream);
		m_kFStream = NULL;
	}
}

void UT_OperateLogStreamFileOSS::RecordLogMessage( char* pCLogMsg )
{
	if( !m_nLogPathState )
	{
		return;
	}

	//得到新log文件名字
	wchar_t szFileName[256] = {0};
	swprintf_s(szFileName, 256, L"%s%s", m_szFilePath, GetLogFileName());

	if(m_kFileName != szFileName)
	{
		if(m_kFStream){
			fclose(m_kFStream);
		}
		//修正中文目录无法创建Log文件的问题
		//CP936_BEGIN_
			m_kFStream = _wfopen(szFileName, L"a");
		//CP936_END_
			if(m_kFStream){
				fputs(LOG_SPLIT_LINE, m_kFStream);
				fputs("\n", m_kFStream);
				fputs(LOG_SPLIT_ELEM, m_kFStream);
				fputs(GET_CUR_VERSION_INFO(), m_kFStream);
				fputs(LOG_SPLIT_ELEM, m_kFStream);
				fputs("\n", m_kFStream);
				fputs(LOG_SPLIT_LINE, m_kFStream);
				fputs("\n", m_kFStream);
			}
	}
	if(m_kFStream)
	{
		fputs(GetCurDateTimeStr(),m_kFStream);
		fputs("|",m_kFStream);
		std::string sLogMsg = pCLogMsg;
		fwrite(pCLogMsg,sLogMsg.length(),1,m_kFStream);
		fputs("\n",m_kFStream);
		time_t sec = time(0);
		if(sec - m_secs > 2)
		{
			fflush(m_kFStream);
			m_secs = sec;
		}
		m_kFileName = szFileName;
	}
}


void UT_OperateLogStreamFileOSS::MakeDirectory(const wchar_t * strLogDir)
{
	std::wstring strLogPath = strLogDir;
	if(strLogPath.length() <= 0)
	{
		strLogPath = L"./log/";
	}

	const wchar_t lastKey = strLogPath[strLogPath.length() - 1];
	if(lastKey != L'/' && lastKey != L'\\')
	{
		strLogPath = strLogPath + L"/";
	}
	if(strLogPath[0] == L'.')
	{
		std::wstring tmpDir = strLogPath.substr(1, strLogPath.length());
		wchar_t szFile[200];
		_wgetcwd(szFile, sizeof(szFile));
		strLogPath = szFile + tmpDir;
	}

	//创建OSS目录
	swprintf_s(m_szFilePath , _countof(m_szFilePath), L"%s%s/", strLogPath.c_str(),m_strPathName.c_str());
#ifdef WIN32
	int n = wcslen(m_szFilePath);
	for(int i=0;i<n; i++)
	{
		if(m_szFilePath[i] == L'/')
		{
			m_szFilePath[i] = L'\\';
		}
	}
#endif
	if(!MakeDir(m_szFilePath))  //失败则写top日志
	{
		m_nLogPathState = 0;
		return;
	}

	m_nLogPathState = 1;
}


char * UT_OperateLogStreamFileOSS::GetDateTimeStr(time_t *mytime)
{
	static char szTimeToSec[50];
	struct tm curr;

	localtime_s(&curr, mytime);

	if (curr.tm_year > 50)
	{
		sprintf_s(szTimeToSec, sizeof(szTimeToSec), "%04d-%02d-%02d %02d:%02d:%02d", 
			curr.tm_year+1900, curr.tm_mon+1, curr.tm_mday,
			curr.tm_hour, curr.tm_min, curr.tm_sec);
	}
	else
	{
		sprintf_s(szTimeToSec, sizeof(szTimeToSec), "%04d-%02d-%02d %02d:%02d:%02d",
			curr.tm_year+2000, curr.tm_mon+1, curr.tm_mday,
			curr.tm_hour, curr.tm_min, curr.tm_sec);
	}

	return szTimeToSec;
}


char * UT_OperateLogStreamFileOSS::GetCurDateTimeStr(void)
{
	time_t  iCurTime;

	time(&iCurTime);
	return GetDateTimeStr(&iCurTime);
}



wchar_t * UT_OperateLogStreamFileOSS::GetLogFileName()
{
	static wchar_t szTime[50] = {0};

	time_t  iCurTime;

	time(&iCurTime);

	struct tm curr;

	localtime_s(&curr, &iCurTime);

	swprintf_s(szTime, _countof(szTime), L"%d_%04d%02d%02d%02d.log", 
		m_serverId,curr.tm_year+1900, curr.tm_mon+1, curr.tm_mday,
		curr.tm_hour);

	return szTime;
}

void OPERATELOG::GetOperateLogPrefix(char* sText, LC_ServerPlayer* pOwner)
{
	if(NULL == pOwner)
	{
		return;
	}
	//gamesrvid | userid | username | accountname |

	int32_t nSrvId = GetGameServerEntryID();
	int32_t nUserId = pOwner->GetUserID();
	int32_t nCId = pOwner->GetID();
	StringType sAccountName = pOwner->GetUserName();
	StringType sUserName = pOwner->GetCharName();
	_snprintf(sText, 256, "%d|%d|%d|%s|%s", nSrvId, nUserId,nCId ,sUserName.c_str(),sAccountName.c_str());
}

void OPERATELOG::RecordItemOperateLog(LC_PlayerBase* pkPlayer, int nOperateLogCode, int nType, int nItemId, int nItemCount,int nBeforCount, const std::vector<int>&Params /* = std::vector<int>() */)
{
	if(nOperateLogCode == 0)
		return;
	LC_ServerPlayer* pSrvPlayer = dynamic_cast<LC_ServerPlayer*>(pkPlayer);
	if (NULL == pSrvPlayer)
	{
		return;
	}

	LC_ServerPlayer* pOwner = pSrvPlayer->GetOwnerPlayer();
	if(NULL == pOwner)
	{
		return;
	}
	char cLogMsg[1024] = {'\0'};
	char cText[256] = {'\0'};
	GetOperateLogPrefix(cText, pOwner);
	if(Params.size() > 0) 
	{
		std::string  sMsg = "";
		char sParam[32] = {'\0'};
		for(int i = 0; i < Params.size(); i++) 
		{
			_snprintf(sParam, 32, "|%d", Params[i]);
			sMsg += sParam;
		}
		if(nOperateLogCode == 100000 || nOperateLogCode == 100001)
		{
			_snprintf(cLogMsg, 1024, "%s|%d|%d%s|%d|%d|%d%s", cText, nType, nOperateLogCode,sMsg.c_str(), nItemId, nItemCount, nBeforCount, sMsg.c_str());
		}else
		{
			_snprintf(cLogMsg, 1024, "%s|%d|%d|%d|%d|%d|%d%s", cText, nType, nOperateLogCode, 0, nItemId, nItemCount, nBeforCount, sMsg.c_str());
		}
		
	}
	else
	{
		_snprintf(cLogMsg, 1024, "%s|%d|%d|%d|%d|%d|%d", cText, nType, nOperateLogCode, 0, nItemId, nItemCount, nBeforCount);
	}
	UT_OperateLogStreamFileOSS::GetSingletonPtr()->RecordLogMessage(cLogMsg);

}

void OPERATELOG::RecordOperateLog(LC_PlayerBase* pkPlayer, int nType, int nOperateLogCode, const std::vector<uint64_t>&Params /* = std::vector<int>() */)
{
	if(nOperateLogCode == 0)
		return;
	LC_ServerPlayer* pSrvPlayer = dynamic_cast<LC_ServerPlayer*>(pkPlayer);
	if (NULL == pSrvPlayer)
	{
		return;
	}

	LC_ServerPlayer* pOwner = pSrvPlayer->GetOwnerPlayer();
	if(NULL == pOwner)
	{
		return;
	}
	char cLogMsg[1024] = {'\0'};
	char cText[256] = {'\0'};
	GetOperateLogPrefix(cText, pOwner);
	if(Params.size() > 0) 
	{
		std::string  sMsg = "";
		char sParam[32] = {'\0'};
		for(int i = 0; i < Params.size(); i++) 
		{
			_snprintf(sParam, 32, "|%lld", Params[i]);
			sMsg += sParam;
		}
		_snprintf(cLogMsg, 1024, "%s|%d|%d|%d%s", cText,nType, nOperateLogCode, 0, sMsg.c_str());
	}
	else
	{
		_snprintf(cLogMsg, 1024, "%s|%d|%d|%d", cText, nType, nOperateLogCode,0);
	}
	UT_OperateLogStreamFileOSS::GetSingletonPtr()->RecordLogMessage(cLogMsg);

}

void OPERATELOG::RecordCashOperateLog(LC_PlayerBase* pkPlayer, int nOperateLogCode, int nType, int nCashType, int nCount,int nBeforCount, const std::vector<int>&Params /* = std::vector<int>() */)
{
	if(nOperateLogCode == 0 || nCount == 0)
		return;
	LC_ServerPlayer* pSrvPlayer = dynamic_cast<LC_ServerPlayer*>(pkPlayer);
	if (NULL == pSrvPlayer)
	{
		return;
	}

	LC_ServerPlayer* pOwner = pSrvPlayer->GetOwnerPlayer();
	if(NULL == pOwner)
	{
		return;
	}
	char cLogMsg[1024] = {'\0'};
	char cText[256] = {'\0'};
	GetOperateLogPrefix(cText, pOwner);

	//当前剩余数量
	if(nBeforCount < 0)
	{
		switch(nCashType) 
		{ 
		case SPEC_ITEM_TID_RUNE_CREAM:
			{ 
				nCashType = CT_RUNE_CREAM;
			} break;
		case SPEC_ITEM_TID_STAR_SOUL:
			{ 
				nCashType = CT_STAR_SOUL;
			} break;
		case SPEC_ITEM_TID_BUDDY_EXP:
			{ 
				nCashType = CT_BUDDY_EXP;
			} break;
		case SPEC_ITEM_TID_CASH:
			{ 
				nCashType = CT_UNBIND_CASH;
			} break;
		case SPEC_ITEM_TID_AP:
			{ 
				nCashType = CT_ARENA_POINT;
			} break;
		case SPEC_ITEM_TID_YUANBAO:
			{ 
				nCashType = CT_UNBIND_YUANBAO;
			} break;
		case SPEC_ITEM_TID_LIJIN:
			{ 
				nCashType = CT_BIND_YUANBAO;
			} break;
		case SPEC_ITEM_TID_SKIN_POINT:
			{ 
				nCashType = CT_SKIN_POINT;
			} break;
		case SPEC_ITEM_TID_JUSTICE:
			{ 
				nCashType = CT_JUSTICE_PT;
			} break;
		case SPEC_ITEM_TID_COURAGE_S:
			{ 
				nCashType = CT_COURAGE_PT;
			} break;
		case SPEC_ITEM_TID_PENTACLE:
			{ 
				nCashType = CT_PENTACLE;
			} break;
		case SPEC_ITEM_TID_ENERGY:
			{ 
				nCashType = CT_ENERGY;
			} break;
		case SPEC_ITEM_TID_TRAIL:
			{ 
				nCashType = CT_TRIAL;
			} break;
		case SPEC_ITEM_TID_MAGIC_CUBE:
			{ 
				nCashType = CT_MAGIC_CUBE;
			} break;
		case SPEC_ITEM_TID_ACTIVE_MERIT:
			{ 
				nCashType = CT_ACTIVE_MERIT;
			} break;
		case SPEC_ITEM_TID_REWARD_MEDAL:
			{ 
				nCashType = CT_REWARD_MEDAL;
			} break;
		case SPEC_ITEM_TID_DAILY_MERIT:
			{ 
				nCashType = CT_DAILY_MERIT;
			} break;
		case SPEC_ITEM_TID_GUILD_MERIT:
			{ 
				nCashType = CT_GUILD_MERIT;
			} break;
		case SPEC_ITEM_TID_CHAPTER_HONOUR:
			{ 
				nCashType = CT_CHAPTER_HONOUR;
			} break;
		case SPEC_ITEM_TID_BOSS_JP:
			{ 
				nCashType = CT_BOSS_JP;
			} break;
		case SPEC_ITEM_TID_ACCOUNT_POINT:
			{ 
				nCashType = CT_ACCOUNT_POINT;
			} break;
		case SPEC_ITEM_TID_YIJITUXI_POINT:
			{ 
				nCashType = CT_CASH_TYPE_29;
			} break;
		case SPEC_ITEM_TID_YUANZHENG_POINT:
			{ 
				nCashType = CT_CASH_TYPE_28;
			} break;
		case SPEC_ITEM_TID_CT_CASH_TYPE30:
			{ 
				nCashType = CT_CASH_TYPE_30;
			} break;
		case SPEC_ITEM_TID_CT_CASH_TYPE31:
			{ 
				nCashType = CT_CASH_TYPE_31;
			} break;
		case	SPEC_ITEM_TID_CT_CASH_TYPE32:
			{
				nCashType = CT_SKYARENA_POINT;
			} break;
		case SPEC_ITEM_TID_CT_CASH_TYPE34:
			{
				nCashType = CT_CASH_TYPE_34;
			} 
			break;
		case SPEC_ITEM_TID_CT_CASH_TYPE35:
			{
				nCashType = CT_CASH_TYPE_35;
			} 
			break;
		case SPEC_ITEM_TID_CT_CASH_TYPE36:
			{
				nCashType = CT_CASH_TYPE_36;
			} 
			break;
		case SPEC_ITEM_TID_CT_CASH_TYPE37:
			{
				nCashType = CT_CASH_TYPE_37;
			}
			break;
		}
		nBeforCount = pOwner->GetCashNumByCashType(static_cast<CashType>(nCashType));
	}
	if(Params.size() > 0) 
	{
		std::string  sMsg = "";
		char sParam[32] = {'\0'};
		for(int i = 0; i < Params.size(); i++) 
		{
			_snprintf(sParam, 32, "|%d", Params[i]);
			sMsg += sParam;
		}
		if(nOperateLogCode == 100000 || nOperateLogCode == 100001)
		{
			_snprintf(cLogMsg, 1024, "%s|%d|%d%s|%d|%d|%d%s", cText, nType, nOperateLogCode, sMsg.c_str(), nCashType, nCount, nBeforCount, sMsg.c_str());
		}else
		{
			_snprintf(cLogMsg, 1024, "%s|%d|%d|%d|%d|%d|%d%s", cText, nType, nOperateLogCode, 0, nCashType, nCount, nBeforCount, sMsg.c_str());
		}
	}
	else
	{
		_snprintf(cLogMsg, 1024, "%s|%d|%d|%d|%d|%d|%d", cText, nType, nOperateLogCode, 0, nCashType, nCount, nBeforCount);
	}
	UT_OperateLogStreamFileOSS::GetSingletonPtr()->RecordLogMessage(cLogMsg);

}


//////////////////////////////////////////////////////////////////////////








