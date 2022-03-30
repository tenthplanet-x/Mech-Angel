
#include "UT_GameServerProfile.h"
#include "TM_ServerTimeManager.h"

#include "UT_Timer.h"
#include "UT_Log.h"
using namespace Utility;

//----------------------------------------------------------
UT_GameServerProfile::UT_GameServerProfile(const StringType& rkNodeName,float fTolerantTime)
{
	m_uiStartTime = TICK_CURRENT_TIMESTAMP();
	m_uiTolerantTime = (uint64_t)(fTolerantTime * ONE_SECOND_MICRO);
	m_kNodeName = rkNodeName;
}
//----------------------------------------------------------
UT_GameServerProfile::~UT_GameServerProfile()
{	
	uint64_t curtimestamp = TICK_CURRENT_TIMESTAMP();
	if(curtimestamp >= m_uiStartTime + m_uiTolerantTime)
		GfxWriteLog(LOG_RES_FILELINE,LOG_SWITCH_RES,"[Profile]<%s> %lld", m_kNodeName.c_str(), curtimestamp-m_uiStartTime);
}