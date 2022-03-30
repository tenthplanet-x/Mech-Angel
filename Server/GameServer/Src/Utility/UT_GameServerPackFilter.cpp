
#include "GameServerApp.h"
#include "UT_GameServerPackFilter.h"

using namespace Utility;
//---------------------------------------------------------------------
UT_GameServerPackFilter::UT_GameServerPackFilter()
{
}
//---------------------------------------------------------------------
UT_GameServerPackFilter::~UT_GameServerPackFilter()
{

}
//---------------------------------------------------------------------
void UT_GameServerPackFilter::Reset()
{
	m_uiStartRecordTime = 0;
	_clearRecord();
}
//---------------------------------------------------------------------
void UT_GameServerPackFilter::SetGlobalFilterFrequency(uint32_t uiFrequency)
{
	m_kGlobalRecord.m_uiFrequency	= uiFrequency;
	m_kGlobalRecord.m_uiRecord		= 0;
}
//---------------------------------------------------------------------
void UT_GameServerPackFilter::AddSpecialFilterFrequency(uint32_t uiPackType,uint32_t uiFrequency)
{
	_filterRecord& kRecord = m_kSpecialPackFilterRecord[uiPackType];
	kRecord.m_uiFrequency = uiFrequency;
	kRecord.m_uiRecord	  = 0;
}
//---------------------------------------------------------------------
bool UT_GameServerPackFilter::FilterPacket(uint32_t uiPackType)
{
	uint32_t uiCurrentTime = (uint32_t)GET_PROCESS_TIME;
	if(uiCurrentTime != m_uiStartRecordTime)
	{
		_clearRecord();
		m_uiStartRecordTime = uiCurrentTime;
	}
	
	m_kGlobalRecord.m_uiRecord += 1;
	if(m_kGlobalRecord.m_uiRecord > m_kGlobalRecord.m_uiFrequency)
		return true;

	UT_SpecialPackFilterRecordIter IterFind = m_kSpecialPackFilterRecord.find(uiPackType);
	if(IterFind != m_kSpecialPackFilterRecord.end())
	{
		_filterRecord& rkRecord = IterFind->second;
		rkRecord.m_uiRecord += 1;
		if(rkRecord.m_uiRecord > rkRecord.m_uiFrequency)
			return true;
	}

	return false;
}
//---------------------------------------------------------------------
void UT_GameServerPackFilter::_clearRecord()
{
	m_kGlobalRecord.m_uiRecord		= 0;

	UT_SpecialPackFilterRecordIter IterBe = m_kSpecialPackFilterRecord.begin();
	UT_SpecialPackFilterRecordIter IterEd = m_kSpecialPackFilterRecord.end();
	for( ; IterBe != IterEd; ++IterBe)
	{
		_filterRecord& rkRecord = IterBe->second;
		rkRecord.m_uiRecord		= 0;
	}
}