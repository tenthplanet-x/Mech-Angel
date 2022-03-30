#include "ClientPCH.h"
#include "YunYingMgr.h"
#include "MG_MessageBase.h"
#include "LianYun/duowan/include/yggamemsg_loadlibrary.h"
#include <process.h>
#include "NW_ClientSessionManager.h"	//< PS_CharViewInfo
#include "LC_ClientLogicManager.h"

//--------------------------------------------------------

LC_YunYingManager::LC_YunYingManager()
{
	if (YYP_DUOWAN == getYunYingplatform())
	{
		bool b = YG_GAMEMSG::yggamemsg_init(L"yggamemsg.dll");
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_TAG_SYSTEM, "duowan yggamemsg.dll load %s", b ? "true" : "false");
	}
}

LC_YunYingManager::~LC_YunYingManager()
{
	if (YYP_DUOWAN == getYunYingplatform())
		YG_GAMEMSG::yggamemsg_uninit();

	hideWebWindow();
}

bool LC_YunYingManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK,				_event_TimerTick)
		MESSAGE_HANDLE(MGT_YUNYING_NEW_ROLE,		_event_NewRole)
		MESSAGE_HANDLE(MGT_YUNYING_PLAYER_LOGIN,	_event_GamePlayerLogin)
		MESSAGE_HANDLE(MGT_YUNYING_PLAYER_LEVELUP,	_event_GamePlayerLevelUp)
		MESSAGE_HANDLE(MGT_YUNYING_PLAYER_QUIT,		_event_GamePlayerQuit)
		MESSAGE_HANDLE(MGT_YUNYING_PLAYER_CONSUME,	_event_GamePlayerConsume)
	END_MESSAGE_HANDLE
	return true;
}

void LC_YunYingManager::_event_TimerTick( MG_MessageBase& rkMessage )
{
	//////////////////////////////////////////////////////////////////////////
	//< tx对接
	if (YYP_TX == getYunYingplatform())
	{
		static int		ls_txCount = 0;
		static DWORD	ls_txLastKeepAliveTime = timeGetTime();
		if (++ls_txCount > 12000) //< fps60: 200s; fps30, 400s; fps20, 600s
		{
			ls_txCount = 0;

			DWORD ct = timeGetTime();
			if (ct - ls_txLastKeepAliveTime > 3600 * 1000)	//< 每个1~2小时上报
			{
				ls_txLastKeepAliveTime = ct;
				report2tx_keepalive();
			}
		}
	}
}

void LC_YunYingManager::_event_NewRole( MG_MessageBase& rkMessage )
{
	const PS_CharViewInfo * pkCharInfo = (const PS_CharViewInfo *)rkMessage.m_pbyParam1;
	const PS_CharViewInfo & rkCharInfo = *pkCharInfo;

	YunYingReportArg::singleton().setnewrole(true);
	YunYingReportArg::singleton().setnewrole_regtime(rkCharInfo.m_iRegTime);
	YunYingReportArg::singleton().setgame2newrole3min_reported(false);
	YunYingReportArg::singleton().setgame2newrole3min_begintime(timeGetTime());

	//////////////////////////////////////////////////////////////////////////

	unique_id_impl	uuid_impl = GEN_UNIQUE_ID(getYunYingplatform(), rkCharInfo.m_iCitizenship, rkCharInfo.m_uiCharID);
	//< tx对接
	report2tx_register(rkCharInfo.m_uiCharID);
	//< duowan对接
	report2duowan_register((unique_id_type)uuid_impl, rkCharInfo.m_szCharName, rkCharInfo.m_byCharType, rkCharInfo.m_usCharLevel, rkCharInfo.m_byWeapon);
}

void LC_YunYingManager::_event_GamePlayerLogin( MG_MessageBase& rkMessage )
{
	const PS_CharViewInfo * pkCharInfo = (const PS_CharViewInfo *)rkMessage.m_pbyParam1;
	const PS_CharViewInfo & rkCharInfo = *pkCharInfo;

	unique_id_impl	uuid_impl = GEN_UNIQUE_ID(getYunYingplatform(), rkCharInfo.m_iCitizenship, rkCharInfo.m_uiCharID);
	//< tx对接
	report2tx_login(rkCharInfo.m_uiCharID, rkCharInfo.m_usCharLevel);
	//< 52xiyou对接
	report2xy_login(StringType(rkCharInfo.m_szCharName), rkCharInfo.m_usCharLevel, rkCharInfo.m_byWeapon);
}

void LC_YunYingManager::_event_GamePlayerLevelUp( MG_MessageBase& rkMessage )
{
	int  newLV			= rkMessage.m_lParam1;
	unique_id_type  uuid=*(unique_id_type *)rkMessage.m_pbyParam2;
	BYTE*role_name		= rkMessage.m_pbyParam3;
	int  role_gender	= rkMessage.m_lParam4;
	int  role_profession= rkMessage.m_lParam5;
	int  role_power		= rkMessage.m_lParam6;


	//report2360wan_levelup(newLV, uuid, (const char *)role_name, role_gender, role_profession, role_power);
	//report2360youxi_levelup(newLV, uuid, (const char *)role_name, role_gender, role_profession, role_power);
	report2duowan_async_levelup(newLV, uuid, (const char *)role_name, role_gender, role_profession, role_power);
}

void LC_YunYingManager::_event_GamePlayerQuit( MG_MessageBase& rkMessage )
{

}

void LC_YunYingManager::_event_GamePlayerConsume( MG_MessageBase& rkMessage )
{

}

//////////////////////////////////////////////////////////////////////////
