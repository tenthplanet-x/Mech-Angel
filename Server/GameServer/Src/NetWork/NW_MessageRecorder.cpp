
#include "NW_MessageRecorder.h"
#include "UT_Log.h"
#include "UT_TimeUtil.h"
using namespace Utility;

NW_MessageRecorder::NW_MessageRecorder(void)
:m_beginTime(0)
// ,m_bpsControlor(65535)
{
	// µ¥Î»byte
// 	m_bpsControlor[MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_TARGET].m_BPS			= 1000;
// 	m_bpsControlor[MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_DIR].m_BPS			= 1000;
// 	m_bpsControlor[MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_LOCATION].m_BPS		= 1000;
// 	m_bpsControlor[MGPT_SYNC_PLAYERS_PROPERTY_ENTER_IN_FOV].m_BPS			= 400;
// 	m_bpsControlor[MGPT_SYNC_PLAYERS_STATECHANGE_IN_FOV].m_BPS				= 300;
// 	m_bpsControlor[MGPT_SYNC_PLAYERS_LOCATION_IN_FOV].m_BPS					= 1100;
}

NW_MessageRecorder::~NW_MessageRecorder(void)
{
}

void NW_MessageRecorder::AddMessage( int32_t messageType,char *pMsg, WORD wSize)
{
	if (0 == m_beginTime)
	{
		m_beginTime = GET_INTEGER_TIME64;
	}
	if (messageType > 0 )
	{
		MessageCountMap::iterator it = m_messageCounter.find(messageType);
		if (it != m_messageCounter.end())
		{
			++((*it).second.m_count);
			(*it).second.m_totalSize += wSize;
		}
		else
		{
			MessageCount& message = m_messageCounter[messageType];
			message.m_type = messageType;
			message.m_count = 1;
			message.m_totalSize = wSize;
		}
	}
}

void NW_MessageRecorder::DiscardMessage( int32_t messageType,char *pMsg, WORD wSize )
{
	if (0 == m_beginTime)
	{
		m_beginTime = GET_INTEGER_TIME64;
	}
	if (messageType > 0 )
	{
		MessageCountMap::iterator it = m_discardMessageCounter.find(messageType);
		if (it != m_discardMessageCounter.end())
		{
			++((*it).second.m_count);
			(*it).second.m_totalSize += wSize;
		}
		else
		{
			MessageCount message;
			message.m_type = messageType;
			message.m_count = 1;
			message.m_totalSize = wSize;
			m_discardMessageCounter.insert(std::make_pair(messageType,message));
		}
	}
}

void NW_MessageRecorder::Output(size_t playerCount)
{
	GfxWriteLog(LOG_TAG_RES,LOG_SWITCH_RES,"---------------------- send message -------------------------------");

	int64_t endTime = GET_INTEGER_TIME64;
	size_t totalSize = 0;
	size_t messagCount = 0;
	MessageCountList kMsgCnts;
	{
		for (MessageCountMap::iterator it = m_messageCounter.begin(); it != m_messageCounter.end(); ++it)
		{
			kMsgCnts.push_back(it->second);
		}
	}
	kMsgCnts.sort();

	{
		for (MessageCountList::reverse_iterator it = kMsgCnts.rbegin(); it != kMsgCnts.rend(); ++it)
		{
			GfxWriteLog(LOG_TAG_RES,LOG_SWITCH_RES,"id = %d		count = %u	size = %u",it->m_type,it->m_count,it->m_totalSize);
			totalSize += it->m_totalSize;		
			messagCount += it->m_count;
		}
	}
	int time = endTime - m_beginTime;
	if(time <= 0)
		return;

	float floatSecondTime = (float(time) / float(1000));
	int bps = int(float(totalSize) / floatSecondTime * 8 );
	size_t noHeadSize = totalSize - (messagCount /1024 * 32) ;
	int noHeadbps = int(float(noHeadSize) / floatSecondTime * 8);
	GfxWriteLog(LOG_TAG_RES,LOG_SWITCH_RES,"milliseconds  = %u	size = %u	count = %u	bps = %d	playerCount = %u" ,
		time,totalSize,messagCount,bps,playerCount);
	GfxWriteLog(LOG_TAG_RES,LOG_SWITCH_RES,"no head:	size = %u	bps = %d",
		noHeadSize,noHeadbps);


	GfxWriteLog(LOG_TAG_RES,LOG_SWITCH_RES,"---------------------- discard message -------------------------------");
	totalSize = 0;
	messagCount = 0;
	for (MessageCountMap::iterator it = m_discardMessageCounter.begin(); it != m_discardMessageCounter.end(); ++it)
	{
		GfxWriteLog(LOG_TAG_RES,LOG_SWITCH_RES,"id = %d		count = %u	size = %u",(*it).first,(*it).second.m_count,(*it).second.m_totalSize);
		totalSize += (*it).second.m_totalSize;		
		messagCount += (*it).second.m_count;
	}
	bps = int(float(totalSize) / floatSecondTime * 8 );
	noHeadSize = totalSize - (messagCount /1024 * 32);
	noHeadbps = int(float(noHeadSize) / floatSecondTime * 8);
	GfxWriteLog(LOG_TAG_RES,LOG_SWITCH_RES,"milliseconds  = %u	size = %u	count = %u	bps = %d",
		time,totalSize,messagCount,bps );
	GfxWriteLog(LOG_TAG_RES,LOG_SWITCH_RES,"no head:	size = %u	bps = %d",
		noHeadSize,noHeadbps);

}

void NW_MessageRecorder::Clear()
{
	m_beginTime = 0;
	m_messageCounter.clear();
	m_discardMessageCounter.clear();
}

bool NW_MessageRecorder::CanSend( int32_t messageType,WORD wSize )
{
	return true;
// 	return m_bpsControlor[messageType].CanSend(wSize);
}

NW_MessageRecorder::MessageCount::MessageCount()
:m_type(0)
,m_count(0)
,m_totalSize(0)
{
}
NW_MessageRecorder::MessagebpsControlor::MessagebpsControlor()
:m_BPS(6553500)
{

}
bool NW_MessageRecorder::MessagebpsControlor::CanSend( WORD wSize )
{
	return wSize <= m_BPS;
}

