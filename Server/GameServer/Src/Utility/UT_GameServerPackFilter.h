#ifndef _UT_GAME_SERVER_PACK_FILTER_H
#define _UT_GAME_SERVER_PACK_FILTER_H

#include "MM_BaseObject.h"


class UT_GameServerPackFilter : public Memory::MM_BaseObject
{
public:
	UT_GameServerPackFilter();
	~UT_GameServerPackFilter();
	
	void  Reset();
	void  SetGlobalFilterFrequency(uint32_t uiFrequency);
	void  AddSpecialFilterFrequency(uint32_t uiPackType,uint32_t uiFrequency);
	bool  FilterPacket(uint32_t uiPackType);

private:
	
	void  _clearRecord();


	struct _filterRecord
	{
		_filterRecord()
		{
			m_uiFrequency = 0;
			m_uiRecord    = 0;
		}
		uint32_t	m_uiFrequency;
		uint32_t	m_uiRecord;
	};
	
	typedef mem::map<uint32_t,_filterRecord>			UT_SpecialPackFilterRecord;
	typedef UT_SpecialPackFilterRecord::iterator		UT_SpecialPackFilterRecordIter;

	_filterRecord						m_kGlobalRecord;
	UT_SpecialPackFilterRecord			m_kSpecialPackFilterRecord;
	uint32_t							m_uiStartRecordTime;
};

#endif