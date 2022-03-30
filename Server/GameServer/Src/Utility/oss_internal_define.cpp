#include "oss_internal_define.h"
#include "OSSDefine.h"
#include "GameServerApp.h"
#include "string.h"

#define CHECKNULLPTR(ptr)\
	if(NULL == ptr)\
	return;

//游戏用户登录服务器
void oss_internal_log_user_server_login(LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);

	if(!player->IsMainController())
	{
		return;
	}

	NW_ServerSession* pkSession = player->GetSession();
	if(NULL == pkSession)
	{
		return;
	}

	StringType pid = player->GetUserName();

	char buff1[30];
	char buff2[30];
	int64_t loginTime = player->GetAccountLoginTime();
	std::string a = itoa(loginTime, buff1, 10);
	std::string b = itoa(pkSession->GetSessionIndex(), buff2, 10);
	std::string sessionIdString = a+b;

	int64_t registerTime = player->GetRegTime();
	int32_t playerLevel = player->GetLevel();
	int64_t lastLoginTime = player->GetAccountLoginTime();
	
	OSS_INTERNAL_LOG_INFO(get_world_id(player), LOGIN<<get_world_id(player)<<player->GetInternal()<<pid<<loginTime<<sessionIdString<<registerTime<<playerLevel<<lastLoginTime);
}

//游戏用户登出
void oss_internal_log_user_server_logout(LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);

	if(!player->IsMainController())
	{
		return;
	}

	NW_ServerSession* pkSession = player->GetSession();
	if(NULL == pkSession)
	{
		return;
	}

	char timeBuff[30];
	time_t t = time(0);
	std::string loginOutTime = itoa(t, timeBuff, 10);

	StringType pid = player->GetUserName();
	int64_t loginTime = player->GetAccountLoginTime();

	char buff1[30];
	char buff2[30];
	std::string a = itoa(loginTime, buff1, 10);
	std::string b = itoa(pkSession->GetSessionIndex(), buff2, 10);
	std::string sessionIdString = a+b;//sessionIdString = 登录时间+会话ID的偏移
	int32_t playerLevel = player->GetLevel();

	OSS_INTERNAL_LOG_INFO(get_world_id(player), LOGOUT<<get_world_id(player)<<player->GetInternal()<<pid<<loginTime<<loginOutTime<<sessionIdString<<playerLevel);
}

//用户充值
void oss_internal_log_recharge(LC_ServerPlayer* player, int RMB)
{
	CHECKNULLPTR(player);

	if(!player->IsMainController())
	{
		return;
	}

	char timeBuff[30];
	time_t t = time(0);
	std::string currentTime = itoa(t, timeBuff, 10);
	StringType pid = player->GetUserName();
	OSS_INTERNAL_LOG_INFO(get_world_id(player), RECHARGE<<get_world_id(player)<<player->GetInternal()<<pid<<currentTime.c_str()<<RMB<<currentTime.c_str());
}

//创角
void oss_internal_log_create_role(LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);

	if(!player->IsMainController())
	{
		return;
	}

	char timeBuff[30];
	time_t t = time(0);
	std::string currentTime = itoa(t, timeBuff, 10);
	StringType pid = player->GetUserName();

	OSS_INTERNAL_LOG_INFO(get_world_id(player), CREATE_ROLE<<get_world_id(player)<<player->GetInternal()<<pid<<player->GetInstance()<<currentTime.c_str());
}

//进入游戏
void oss_internal_log_enter_state(LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);

	if(!player->IsMainController())
	{
		return;
	}

	char timeBuff[30];
	time_t t = time(0);
	std::string currentTime = itoa(t, timeBuff, 10);
	StringType pid = player->GetUserName();
	int64_t registerTime = player->GetRegTime();

	OSS_INTERNAL_LOG_INFO(get_world_id(player), ENTER_STAGE<<get_world_id(player)<<player->GetInternal()<<pid<<currentTime.c_str()<<registerTime);
}