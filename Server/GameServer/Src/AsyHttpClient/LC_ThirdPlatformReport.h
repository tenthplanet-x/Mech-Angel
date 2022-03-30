//第三方游戏平台数据上报
#ifndef __LC_THIRD_PLATFORM_REPORT__
#define __LC_THIRD_PLATFORM_REPORT__

#include "AH_AsyHttpClient.h"
#include "LC_ServerPlayer.h"
#include "UT_Singleton.h"
#include "MM_BaseObject.h"

struct http_req_param
{
	http_req_param(StringType _pkKey, StringType _pkValue, bool _bSign)
		:strKey(_pkKey), strValue(_pkValue), bSign(_bSign)
	{
	}
	StringType strKey;
	StringType strValue;
	bool bSign;            //是否参与校验
};

struct  platfom_param
{
	platfom_param()
		:url("")
		,key("")
		,game_name("")
	{

	}
	StringType url;
	StringType key;
	StringType game_name;
};
AJSON_CODE(platfom_param, (url)(key)(game_name))

struct  third_platform_config
{
	platfom_param longzhu;
};
AJSON_CODE(third_platform_config, (longzhu));

struct  http_resp_longzhu
{
	http_resp_longzhu():result(0){}
	int32_t result;
};
AJSON(http_resp_longzhu, (result));

class LC_ThirdPlatformReport : public Memory::MM_BaseObject, public Utility::UT_Singleton<LC_ThirdPlatformReport>
{
public:
	LC_ThirdPlatformReport():m_bLoadFlag(false){}

	void LoadConfig();

	void OnCreateChar(LC_ServerPlayer* pkPlayer);
	void OnCharLevelUp(LC_ServerPlayer* pkPlayer);
	void OnCharLogin(LC_ServerPlayer* pkPlayer);
	void OnCharTotalOnlineTime(LC_ServerPlayer* pkPlayer);
	void OnCharRecharge(LC_ServerPlayer* pkPlayer);

protected:		
	void _concatUriParams(StringType& pkUri, mem::vector<http_req_param>& pkParamVec);
	std::string _longzhuSign(mem::vector<http_req_param>& pkParamVec, StringType pKey);
	void _longzhuReport(LC_ServerPlayer* pkPlayer, uint8_t type);
	void _longzhuResponse(StringType uri, AsyHttpResponse* pkResponse);

private:
	BOOL					m_bLoadFlag;
	third_platform_config	m_thirdPlatformConfig;
};

#endif
