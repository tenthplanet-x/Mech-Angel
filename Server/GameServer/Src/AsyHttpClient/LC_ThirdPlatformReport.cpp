#include "LC_ThirdPlatformReport.h"
#include "GameServerApp.h"
#include "LC_ServerPlayer.h"
#include "UT_MiscFuncs.h"
#include "md5plusplus.h"

void LC_ThirdPlatformReport::LoadConfig()
{
	StringType error_string;
	StringType strPath = "./Data/Config/ThirdPlatform.json";
	if (!boost::ajson::load_from_file(m_thirdPlatformConfig, const_cast<char*>(strPath.c_str()), error_string))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "[error]: load ThirdPlatform.json file error:%s.\n", error_string.c_str());
		return;
	}
	m_bLoadFlag = true;
}

void LC_ThirdPlatformReport::OnCreateChar(LC_ServerPlayer* pkPlayer)
{
	if (!m_bLoadFlag)
	{
		return;
	}
}

void LC_ThirdPlatformReport::OnCharLevelUp(LC_ServerPlayer* pkPlayer)
{
	if (!m_bLoadFlag)
	{
		return;
	}
}

void LC_ThirdPlatformReport::OnCharLogin(LC_ServerPlayer* pkPlayer)
{
	if (!m_bLoadFlag)
	{
		return;
	}
}

void LC_ThirdPlatformReport::OnCharTotalOnlineTime(LC_ServerPlayer* pkPlayer)
{
	if (!m_bLoadFlag)
	{
		return;
	}
}

void LC_ThirdPlatformReport::OnCharRecharge(LC_ServerPlayer* pkPlayer)
{
	if (!m_bLoadFlag)
	{
		return;
	}
}

void LC_ThirdPlatformReport::_concatUriParams(StringType& pkUri, mem::vector<http_req_param>& pkParamVec)
{
	int uCount = 0;
	for (size_t i = 0; i < pkParamVec.size(); ++i)
	{
		if (!uCount)
		{
			pkUri += "?";
		}
		else
		{
			pkUri += "&&";
		}		
		pkUri += pkParamVec[i].strKey;
		pkUri +=  "=";
		pkUri += pkParamVec[i].strValue;
		++uCount;		
	}
}

std::string LC_ThirdPlatformReport::_longzhuSign(mem::vector<http_req_param>& pkParamVec, StringType strKey)
{
	StringType strInput;
	size_t uLen = pkParamVec.size();
	int uCount = 0;
	for (size_t i = 0; i < uLen; ++i)
	{
		if (!pkParamVec[i].bSign)
		{
			continue;
		}
		if (uCount > 0)
		{
			strInput += "&&";
		}
		strInput += pkParamVec[i].strValue;
		++uCount;		
	}	
	strInput += "&&";
	strInput += strKey;
	return getMd5Str(strInput.c_str());
}

void LC_ThirdPlatformReport::_longzhuReport(LC_ServerPlayer* pkPlayer, uint8_t type)
{
	StringType strUri(m_thirdPlatformConfig.longzhu.url);
	StringType userid;
	StringType ThirdUserId;
	StringType roleid;
	StringType roleName;
	StringType ServerId;
	StringType ThirdGrade;
	StringType DailyLoginTimes;
	StringType AccumulateTime;
	StringType Recharge;

	userid = pkPlayer->GetUserName();
	ThirdUserId=Utility::Int2String(pkPlayer->GetUserID());
	roleid=Utility::Int2String(pkPlayer->GetInstanceCharID());
	roleName=pkPlayer->GetOwnerCharName_utf8();
	ServerId=Utility::Int2String(pkPlayer->GetInstanceCitizen());
	ThirdGrade=Utility::Int2String(pkPlayer->GetLevel());

	mem::vector<http_req_param> pkParamVec;
	pkParamVec.reserve(10);
	//涉及签名的参数顺序有要求
	pkParamVec.push_back(http_req_param("userid",userid,true));
	pkParamVec.push_back(http_req_param("GameName",m_thirdPlatformConfig.longzhu.game_name,true));
	pkParamVec.push_back(http_req_param("ServerId",ServerId,true));
	pkParamVec.push_back(http_req_param("ThirdUserId",ThirdUserId,false));
	pkParamVec.push_back(http_req_param("roleid",roleid,false));
	pkParamVec.push_back(http_req_param("YeYouGameCode","5001",false));
	switch(type)
	{
	case  1:
		{
			pkParamVec.push_back(http_req_param("roleName",roleName,false));
			pkParamVec.push_back(http_req_param("BehaviorId","1",false));
		}
		break;
	case  2:
		{
			pkParamVec.push_back(http_req_param("roleName",roleName,false));
			pkParamVec.push_back(http_req_param("BehaviorId","1",false));
			pkParamVec.push_back(http_req_param("ThirdGrade",ThirdGrade,false));
		}
		break;
	case  3:
		{
			LC_CommonStatBasic* pkCmnSTBasic = pkPlayer->GetCommonStatAsset().GetCommonStatBasic(LC_ST_TP_DAILY);
			if (!pkCmnSTBasic)
			{
				return;				
			}
			DailyLoginTimes=Utility::Int2String(pkCmnSTBasic->GetDayLoginCount());
			pkParamVec.push_back(http_req_param("DailyLoginTimes",DailyLoginTimes,false));
			pkParamVec.push_back(http_req_param("BehaviorId","2",false));
		}
		break;
	case  4:
		{
			AccumulateTime=Utility::Int2String(pkPlayer->GetTotalOnlineTime());
			pkParamVec.push_back(http_req_param("AccumulateTime",AccumulateTime,false));
			pkParamVec.push_back(http_req_param("BehaviorId","2",false));
		}
		break;
	case  5:
		{
			LC_VIPAsset& kVipAsset = pkPlayer->GetVIPAsset();
			Recharge=Utility::Int642String(kVipAsset.GetCredit());
			pkParamVec.push_back(http_req_param("Recharge",Recharge,false));
			pkParamVec.push_back(http_req_param("BehaviorId","3",false));
		}
		break;
	default:
		break;
	}

	std::string strSign = _longzhuSign(pkParamVec, m_thirdPlatformConfig.longzhu.key);
	pkParamVec.push_back(http_req_param("code",strSign.c_str(),false));
	_concatUriParams(strUri, pkParamVec);
	
	AsyHttpClient& httpClient = GetGameServerApp()->GetAsyHttpClient();
	httpClient.SendReq((CHAR*) strUri.c_str(),VerbGet,NULL,NULL,NULL,0,AuthSchemaNone, \
		boost::bind(&LC_ThirdPlatformReport::_longzhuResponse, this, strUri, _1));
}

void LC_ThirdPlatformReport::_longzhuResponse(StringType uri, AsyHttpResponse* pkResponse)
{
	INT32 iRet = pkResponse->m_iReturnCode;
	if (HTTP_CLIENT_SUCCESS != iRet)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR, iRet, uri, "longzhu req");	
		return;
	}
	http_resp_longzhu res;
	std::string errorMsg;
	if (!boost::ajson::load_from_buff(res,pkResponse->m_sResult.c_str(),errorMsg))
	{
		return;
	}
	if (res.result != 1)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR, res.result,uri.c_str(), "longzhu ret");	
		return;
	}
}