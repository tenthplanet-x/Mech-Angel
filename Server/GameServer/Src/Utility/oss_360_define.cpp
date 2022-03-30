#include "oss_360_define.h"
#include "OSSDefine.h"
#include "GameServerApp.h"
#include "string.h"

// 360 oss log test URL:
//http://shaka.game-svc.cn/check/check.html

#define CHECKNULLPTR(ptr)\
	if(NULL == ptr)\
	return;

#define		OSS_360_HEADER_SIZE		512

#define		OSS_360_HEADER_FORMAT		"gameinfo interface=%s&gname=sxsy&gid=404&sid=S%d&oldsid=&user=%s&roleid=%s&dept=38&time=%u&username=%s"

static StringType strBuffer;
static char szBuffer[ OSS_360_HEADER_SIZE ];

inline int32_t ip_addr_to_string(uint32_t ip, char* ip_buff, size_t buff_size)
{
	// 192.168.111.222，格式长度为15
	if (NULL == ip_buff || buff_size == 0 || buff_size < 16)
	{
		return -1;
	}
	ip = htonl(ip);
	unsigned char* src = (unsigned char*)&ip;
	const char* fmt = "%u.%u.%u.%u";
	sprintf(ip_buff, fmt, src[0], src[1], src[2], src[3]);
	return 0;
}

char* _oss_360_log_string( const char* szKey, const char* szValue,bool bNoValue = false )
{
	if ( bNoValue )
		sprintf( szBuffer,  "%s=", szKey);
	else
		sprintf( szBuffer,  "%s=%s", szKey, szValue );
	return szBuffer;
}

char* _oss_360_log_uint32( const char* szKey, uint32_t nValue, bool bNoValue = false )
{
	if (bNoValue)
		sprintf( szBuffer,  "%s=", szKey );
	else
		sprintf( szBuffer,  "%s=%u", szKey, nValue );
	return szBuffer;
}

char* _oss_360_log_uint64( const char* szKey, uint32_t nValue, bool bNoValue = false )
{
	if (bNoValue)
		sprintf( szBuffer,  "%s=", szKey );
	else
		sprintf( szBuffer,  "%s=%lld", szKey, nValue );
	return szBuffer;
}

char* _oss_360_log_int32( const char* szKey, int32_t nValue, bool bNoValue = false )
{
	if (bNoValue)
		sprintf( szBuffer,  "%s=", szKey );
	else
		sprintf( szBuffer,  "%s=%d", szKey, nValue );
	return szBuffer;
}

StringType& convert_user_id( StringType& strInput )
{
	//boost::regex pattern("s([^\_]*)\_.*$");
	//strInput = boost::regex_replace( strInput, pattern, "" );
	const char* szPtr = strInput.c_str();
	int nLen = strInput.length();
	StringType strResult = "";
	for ( int i = 0; i < nLen; i++ )
	{
		//if ( szPtr[i] == '_' )
		//	break;
		strResult += szPtr[i];
	}
	strInput = strResult;
	return strInput;
}

char* _oss_360_log_header( const char* szInterface, LC_ServerPlayer* pPlayer = NULL )
{
	//"gameinfo interface=%s&gname=sxsy&gid=404&sid=%d&oldsid=&user=%s&roleid=%s&dept=1&time=%u"
	int nServerId = GetGameServerApp()->GetServerEntryId();
	char szUserId[64] = {'\0'};
	//uint32_t nUserId = 0;
	char szRoleId[32] = {'\0'};
	char szUserName[64] = {'\0'};
	sprintf(szRoleId, "");
	sprintf(szUserId, "");
	sprintf(szUserName, "");
	object_id_type nRoleId = 0;

	// for test begin
	//StringType strTest = "12341131_1";
	//convert_user_id(strTest);
	// for test end

	if ( pPlayer )
	{
		StringType strUser = pPlayer->GetUserName_utf8().c_str();
		convert_user_id( strUser );
		sprintf( szUserId, "%s", strUser.c_str() );
		nRoleId = pPlayer->GetInstanceCharID();
		sprintf( szRoleId, "%u", nRoleId );
		StringType strUserName = pPlayer->GetOwnerCharName().c_str();
		sprintf( szUserName, "%s", strUserName.c_str() );
		
	}
	int nSize = sprintf( szBuffer,  OSS_360_HEADER_FORMAT, szInterface, nServerId, szUserId, szRoleId, time(NULL), szUserName);
	return szBuffer;
}

int convert_reason_gold( int nInput )
{
	int nOutput = 6;
	//status varchar(255) 产生原因 
	//1、捡到 2、打怪 3、做任务（包括副本） 4、跟人交易 5、npc出售 6、其他
	switch ( nInput )
	{
	case SYS_FUNC_TP_PICKUP:
		nOutput = 1;
		break;
	case SYS_FUNC_TP_FARM:
		nOutput = 2;
		break;
	case SYS_FUNC_TP_TASK_FINISH:
		nOutput = 3;
		break;
	case SYS_FUNC_TP_TRADE:
		nOutput = 4;
		break;
	case SYS_FUNC_TP_NPC_SHOP:
		nOutput = 5;
		break;
	default:
		nOutput = 6;
		break;
	}

	return nOutput;
}

////////////////////////////////////////////
//记录首次加载flash之前的新增用户
//字段名称 类型 含义
//channel varchar(255) 渠道号 如取不到，请传“0”.
//自然用户，渠道号为-1
//poster varchar(255) 海报号 如取不到，请传“0”
//site varchar(255) 子站id 如取不到，请传“0”
//ip varchar(255) 用户登录ip
void oss_360_log_game_enter(LC_ServerPlayer* player)
{
	char szIP[32] = { '\0' };
	ip_addr_to_string( player->GetLoginIP(), szIP, sizeof(szIP) );

	StringType strValue1 = _oss_360_log_int32("channel", -1);
	StringType strValue2 = _oss_360_log_uint32("poster", 0);
	StringType strValue3 = _oss_360_log_uint32("site", 0);
	StringType strValue4 = _oss_360_log_string("ip", szIP);
	OSS_360_LOG_INFO(
		_oss_360_log_header( "game_enter", player )
		, strValue1 << strValue2 << strValue3 << strValue4
		);
}

//游戏用户登录服务器
void oss_360_log_user_login(LC_ServerPlayer* player)
{
	CHECKNULLPTR(player);
	if(!player->IsMainController())
		return;

	NW_ServerSession* pkSession = player->GetSession();
	if(NULL == pkSession)
		return;

	char szIP[32] = { '\0' };
	ip_addr_to_string( player->GetLoginIP(), szIP, sizeof(szIP) );

	StringType strValue1 = _oss_360_log_uint32( "level", player->GetLevel());
	StringType strValue2 = _oss_360_log_string("ip", szIP);
	StringType strValue3 = _oss_360_log_uint32( "map_id", player->GetMapResID());
	StringType strValue4 = _oss_360_log_uint64( "sessionId", player->GetSessionId());
	
	OSS_360_LOG_INFO(
		_oss_360_log_header( "login", player ), strValue1 << strValue2 <<strValue3<<strValue4
	);
}

void oss_360_log_user_logout( LC_ServerPlayer* player )
{
	CHECKNULLPTR(player);
	if(!player->IsMainController())
		return;

	NW_ServerSession* pkSession = player->GetSession();
	if(NULL == pkSession)
		return;

	char szIP[32] = { '\0' };
	ip_addr_to_string( player->GetLoginIP(), szIP, sizeof(szIP) );

	StringType strValue1 = _oss_360_log_uint32("level", player->GetLevel());
	StringType strValue2 = _oss_360_log_string("ip", szIP);
	StringType strValue3 = _oss_360_log_uint32("map_id", player->GetMapResID());
	StringType strValue4 = _oss_360_log_uint32("onlinetime", player->GetSessionOnlineTime());
	StringType strValue5 = _oss_360_log_uint64( "sessionId", player->GetSessionId());

	OSS_360_LOG_INFO(
		_oss_360_log_header( "logout", player ), strValue1 << strValue2 <<strValue3 << strValue4<<strValue5;
	);
}

void oss_360_log_online_count(uint32_t nCount)
{
	StringType strOnlineCount = _oss_360_log_uint32("rolecount", nCount);
	OSS_360_LOG_INFO(
		_oss_360_log_header( "online" )
		, strOnlineCount
		);
}

void oss_360_log_user_upgrade( LC_ServerPlayer* pPlayer, int nReason )
{
	CHECKNULLPTR(pPlayer);
	if(!pPlayer->IsMainController())
		return;

	NW_ServerSession* pkSession = pPlayer->GetSession();
	if(NULL == pkSession)
		return;

	StringType strValue1 = _oss_360_log_uint32( "level", pPlayer->GetLevel());
	StringType strValue2 = _oss_360_log_uint32( "map_id", pPlayer->GetMapResID());
	StringType strValue3 = _oss_360_log_string( "rolename", pPlayer->GetOwnerCharName_utf8().c_str() );
	StringType strValue4 = _oss_360_log_uint32( "reason", nReason);

	OSS_360_LOG_INFO(
		_oss_360_log_header( "upgrade", pPlayer ), strValue1 << strValue2 <<strValue3 << strValue4
	);
}

void oss_360_log_create_role( LC_ServerPlayer* pPlayer )
{
	CHECKNULLPTR(pPlayer);
	if(!pPlayer->IsMainController())
		return;

	NW_ServerSession* pkSession = pPlayer->GetSession();
	if(NULL == pkSession)
		return;

	char szIP[32] = { '\0' };
	ip_addr_to_string( pPlayer->GetLoginIP(), szIP, sizeof(szIP) );

	StringType strValue1 = _oss_360_log_string( "rolename", pPlayer->GetOwnerCharName_utf8().c_str() );
	StringType strValue2 = _oss_360_log_uint32( "channel", 0, true);
	StringType strValue3 = _oss_360_log_uint32( "poster", 0, true);
	StringType strValue4 = _oss_360_log_uint32( "site", 0, true);
	StringType strValue5 = _oss_360_log_uint32("prof", pPlayer->GetBornCharType());
	StringType strValue6 = _oss_360_log_string("ip", szIP);
	

	OSS_360_LOG_INFO(
		_oss_360_log_header( "create_role", pPlayer ), strValue1 << strValue2 <<strValue3 << strValue4 << strValue5 << strValue6
	);
}

void oss_360_log_retreat( const char* szResult )
{
	StringType strTime = _oss_360_log_uint32( "timestamp", (uint32_t)time(NULL) );
	StringType strStatus = _oss_360_log_string("status", szResult);
	OSS_360_LOG_INFO(
		_oss_360_log_header( "retreat" )
		, strTime << strStatus
		);
}

void oss_360_log_client_info( LC_ServerPlayer* pPlayer )
{
	OSS_360_LOG_INFO(
		_oss_360_log_header( "client_info", pPlayer )
		, ""
	);
}

void oss_360_log_giftmoney_buy( uint32_t nBanalce, uint32_t nItemId, uint32_t nAmount, uint32_t nMoney, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32("balance", nBanalce);
	StringType strValue2 = _oss_360_log_uint32("itemid", nItemId);
	StringType strValue3 = _oss_360_log_uint32("amount", nAmount);
	StringType strValue4 = _oss_360_log_uint32("money", nMoney);
	StringType strValue5 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	StringType strValue6 = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "giftmoney_buy", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5 << strValue6
	);
}

void oss_360_log_giftmoney_income( uint32_t money, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32("money", money);
	StringType strValue2 = _oss_360_log_uint32("balance", balance);
	StringType strValue3 = _oss_360_log_uint32("reason", reason);
	StringType strValue4 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	StringType strValue5 = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "giftmoney_income", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5
		);
}

void oss_360_log_giftmoney_expend( uint32_t money, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32("money", money);
	StringType strValue2 = _oss_360_log_uint32("balance", balance);
	StringType strValue3 = _oss_360_log_uint32("reason", reason);
	StringType strValue4 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "giftmoney_expend", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4
		);
}

void oss_360_log_gold(uint32_t num, uint32_t status, uint32_t balance, uint32_t type, LC_ServerPlayer* pPlayer)
{
	status = convert_reason_gold( status );
	StringType strValue1 = _oss_360_log_uint32("num", num);
	StringType strValue2 = _oss_360_log_uint32("status", status);
	StringType strValue3 = _oss_360_log_uint32("balance", balance);
	StringType strValue4 = _oss_360_log_uint32("type", type);
	StringType strValue5 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "gold", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5
		);
}

void oss_360_log_vip_privilege(uint32_t nAddVipLevel, LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("addlevel", nAddVipLevel);
	StringType strValue2 = _oss_360_log_uint32("viplevel", pPlayer->GetVIPLevel());
	StringType strValue3 = _oss_360_log_uint32("level", pPlayer->GetVIPLevel());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "vip_privilege", pPlayer )
		, strValue1 << strValue2 << strValue3
		);
}

void oss_360_log_game_buy(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("balance", balance);
	StringType strValue2 = _oss_360_log_uint32("itemid", itemid);
	StringType strValue3 = _oss_360_log_uint32("amount", amount);
	StringType strValue4 = _oss_360_log_uint32("money", money);
	StringType strValue5 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	StringType strValue6 = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "game_buy", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5 << strValue6
		);
}

void oss_360_log_game_sell(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("balance", balance);
	StringType strValue2 = _oss_360_log_uint32("itemid", itemid);
	StringType strValue3 = _oss_360_log_uint32("amount", amount);
	StringType strValue4 = _oss_360_log_uint32("money", money);
	StringType strValue5 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	StringType strValue6 = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "game_sell", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5 << strValue6
		);
}

void oss_360_log_item_log(uint32_t itemid, uint32_t opid, uint32_t amount, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32("itemid", itemid);
	StringType strValue2 = _oss_360_log_uint32("amount", amount);
	StringType strValue3 = _oss_360_log_uint32("opid", opid);
	OSS_360_LOG_INFO(
		_oss_360_log_header( "item_log", pPlayer )
		, strValue1 << strValue2 << strValue3
		);
}

void oss_360_log_task(uint32_t taskid, uint32_t result, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32("taskid", taskid);
	StringType strValue2 = _oss_360_log_uint32("result", result);
	StringType strValue3 = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "task", pPlayer )
		, strValue1 << strValue2 << strValue3
		);
}

void oss_360_log_dupl(uint32_t group_id, int type, uint32_t result, uint32_t dlevel, uint32_t dscore, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32("group_id", group_id);
	StringType strValue2 = _oss_360_log_uint32("result", result);
	StringType strValue3 = _oss_360_log_uint32("type", type);
	StringType strValue4 = _oss_360_log_uint32("dlevel", dlevel);
	StringType strValue5 = _oss_360_log_uint32("dscore", dscore);
	StringType strValue6 = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	StringType strValue7 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "dupl", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 <<strValue5 << strValue6 << strValue7
		);
}

void oss_360_log_vip(uint32_t optype, uint32_t viptype, uint32_t buytime, uint32_t viplv, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32("optype", optype);
	StringType strValue2 = _oss_360_log_uint32("viptype", viptype);
	StringType strValue3 = _oss_360_log_uint32("buytime", buytime);
	StringType strValue4 = _oss_360_log_uint32("viplv", viplv);
	//StringType strMapId = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	//StringType strLevel = _oss_360_log_uint32("level", pPlayer->GetMapResID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "vip", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4
	);
}

void oss_360_log_sign(uint32_t type, LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("type", type);
	OSS_360_LOG_INFO(
		_oss_360_log_header( "sign", pPlayer )
		, strValue1
		);
}

void oss_360_log_equip(uint32_t equipid, uint32_t itemid, uint32_t opid, uint32_t amount, LC_ServerPlayer*  pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("equipid", equipid);
	StringType strValue2 = _oss_360_log_uint32("itemid", itemid);
	StringType strValue3 = _oss_360_log_uint32("opid", opid);
	StringType strValue4 = _oss_360_log_uint32("amount", amount);
	OSS_360_LOG_INFO(
		_oss_360_log_header( "equip", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4
		);
}

void oss_360_log_map( uint32_t level, uint32_t nonce_map, uint32_t to_map, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32("level", level);
	StringType strValue2 = _oss_360_log_uint32("nonce_map", nonce_map);
	StringType strValue3 = _oss_360_log_uint32("to_map", to_map);
	OSS_360_LOG_INFO(
		_oss_360_log_header( "map", pPlayer )
		, strValue1 << strValue2 << strValue3
	);
}

void oss_360_log_warpot(uint32_t before, uint32_t amount, uint32_t after, uint32_t type, LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("before", before);
	StringType strValue2 = _oss_360_log_uint32("amount", amount);
	StringType strValue3 = _oss_360_log_uint32("after", after);
	StringType strValue4 = _oss_360_log_uint32("type", type);
	OSS_360_LOG_INFO(
		_oss_360_log_header( "warpot", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4
	);
}

void oss_360_log_recharge(uint32_t amount, uint32_t money, uint32_t balance, const char*  order, const char* channel, LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("amount", amount);
	StringType strValue2 = _oss_360_log_uint32("money", money);
	StringType strValue3 = _oss_360_log_uint32("balance", balance);
	StringType strValue4 = _oss_360_log_string("order", order);
	StringType strValue5 = _oss_360_log_string("channel", channel);
	StringType strValue6 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	StringType strValue7 = _oss_360_log_uint64("legion", pPlayer->GetGuildID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "recharge", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5 << strValue6 << strValue7
		);
}

void oss_360_log_yb_income(uint32_t amount, uint32_t balance, uint32_t reason, LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("amount", amount);
	StringType strValue2 = _oss_360_log_uint32("balance", balance);
	StringType strValue3 = _oss_360_log_uint32("reason", reason);
	StringType strValue4 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "yb_income", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4
	);
}

void oss_360_log_shop_buy(uint32_t balance, uint32_t itemid, uint32_t amount, uint32_t money, uint32_t remain, uint32_t type, LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("balance", balance);
	StringType strValue2 = _oss_360_log_uint32("itemid", itemid);
	StringType strValue3 = _oss_360_log_uint32("amount", amount);
	StringType strValue4 = _oss_360_log_uint32("money", money);
	StringType strValue5 = _oss_360_log_uint32("remain", remain);
	StringType strValue6 = _oss_360_log_uint32("type", type);
	StringType strValue7 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	StringType strValue8 = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "shop_buy", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5 << strValue6 << strValue7 << strValue8
		);
}

void oss_360_log_yb_expend(uint32_t amount, uint32_t balance, uint32_t reason, uint32_t type,  LC_ServerPlayer* pPlayer)
{
	StringType strValue1 = _oss_360_log_uint32("amount", amount);
	StringType strValue2 = _oss_360_log_uint32("balance", balance);
	StringType strValue3 = _oss_360_log_uint32("reason", reason);
	StringType strValue4 = _oss_360_log_uint32("type", type);
	StringType strValue5 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "yb_expend", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5
	);
}

void oss_360_log_item_income(uint32_t itemid, uint32_t amount, uint32_t reason, uint32_t mode, uint32_t remain,  LC_ServerPlayer* pPlayer)
{
	StringType strBindMode = "";
	if ( mode == 0 )
		strBindMode = "u";
	else
		strBindMode = "b";
	StringType strValue1 = _oss_360_log_uint32("itemid", itemid);
	StringType strValue2 = _oss_360_log_uint32("amount", amount);
	StringType strValue3 = _oss_360_log_uint32("reason", reason);
	StringType strValue4 = _oss_360_log_uint32("remain", remain);
	StringType strValue5 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	StringType strValue6 = _oss_360_log_string( "mode", strBindMode.c_str() );
	OSS_360_LOG_INFO(
		_oss_360_log_header( "item_income", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5 << strValue6
	);
}

void oss_360_log_item_use(uint32_t itemid, uint32_t amount, uint32_t reason, uint32_t mode, uint32_t remain,  uint32_t type,  LC_ServerPlayer* pPlayer)
{
	StringType strBindMode = "";
	if ( mode == 0 )
		strBindMode = "u";
	else
		strBindMode = "b";
	StringType strValue1 = _oss_360_log_uint32("itemid", itemid);
	StringType strValue2 = _oss_360_log_uint32("amount", amount);
	StringType strValue3 = _oss_360_log_uint32("reason", reason);
	StringType strValue4 = _oss_360_log_uint32("remain", remain);
	StringType strValue5 = _oss_360_log_uint32("type", type);
	StringType strValue6 = _oss_360_log_uint32("level", pPlayer->GetLevel());
	StringType strValue7 = _oss_360_log_string( "mode", strBindMode.c_str() );
	StringType strValue8 = _oss_360_log_uint32("map_id", pPlayer->GetMapResID());
	OSS_360_LOG_INFO(
		_oss_360_log_header( "item_use", pPlayer )
		, strValue1 << strValue2 << strValue3 << strValue4 << strValue5 << strValue6  << strValue7 << strValue8
	);
}


void oss_360_log_change_name(LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_string( "name", pPlayer->GetOwnerCharName_utf8().c_str() );
	OSS_360_LOG_INFO(
		_oss_360_log_header( "change_name", pPlayer )
		, strValue1
		);
}
void oss_360_log_level_up(int level, LC_ServerPlayer* pPlayer )
{
	StringType strValue1 = _oss_360_log_uint32( "level", level);
	StringType strValue2 = _oss_360_log_uint32( "time", time(NULL));
	OSS_360_LOG_INFO(
		_oss_360_log_header( "level_up", pPlayer )
		, strValue1<<strValue2
		);
}