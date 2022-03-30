#include "MG_TYPES.h"
#include "NW_InputMessageControlTable.h"

NW_InputMessageControlTable::NW_InputMessageControlTable(void)
:m_inputControlor(65535)
{
	m_inputControlor[MGPT_SYNC_GAME_PLAYER_MOVE].m_frequency = 100;
	m_inputControlor[MGPT_SYNC_GAME_PLAYER_MOVE].m_refresh = 1000;
	m_inputControlor[MGPT_SYNC_GAME_PLAYER_MOVE].m_allowErrorConut = 1;

	m_inputControlor[MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_TARGET].m_frequency = 500;
	m_inputControlor[MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_TARGET].m_refresh = 5000;
	m_inputControlor[MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_TARGET].m_allowErrorConut = 1;
}

NW_InputMessageControlTable::~NW_InputMessageControlTable(void)
{
}

const NW_InputMessageControlTable::InputMessageControlRecord* NW_InputMessageControlTable::GetControlRecord( int32_t messageID )const
{
	if(messageID < 0 || messageID >= (int32_t)(m_inputControlor.size()))
		return NULL;
	
	return &(m_inputControlor[messageID]);
}
NW_InputMessageControlTable::InputMessageControlRecord::InputMessageControlRecord()
:m_frequency(0)
,m_allowErrorConut(1)
,m_refresh(2000)
{
}

NW_InputMessageRecord::NW_InputMessageRecord()
:m_controlTable(NULL)
{

}

void NW_InputMessageRecord::SetControlTable( NW_InputMessageControlTable* controlTable )
{
	m_controlTable = controlTable;
}

bool NW_InputMessageRecord::CheckRecvMessage( int32_t messageType,DWORD time )
{
	bool checked = true;
	const NW_InputMessageControlTable::InputMessageControlRecord* pkControlRecord = m_controlTable->GetControlRecord(messageType);
	if(!pkControlRecord)
	{
		checked = false;
	}
	else if(pkControlRecord->m_frequency != 0)
	{
		InputMessageHistoryMap::iterator it = m_messageHistoryMap.find(messageType);
		if (it != m_messageHistoryMap.end())
		{
			MessageHistory& history = (*it).second;
			if ( (time - history.m_lastInputTime) < pkControlRecord->m_frequency &&
				history.m_errorNumber > pkControlRecord->m_allowErrorConut)
			{
				checked = false;
			}
		}
	}
	return checked;
}

void NW_InputMessageRecord::SetRecvMessage( int32_t messageType,DWORD time )
{
	const NW_InputMessageControlTable::InputMessageControlRecord* pkControlRecord = m_controlTable->GetControlRecord(messageType);
	if(!pkControlRecord)
	{
		return;
	}
	else if (pkControlRecord->m_frequency != 0)
	{
		InputMessageHistoryMap::iterator it = m_messageHistoryMap.find(messageType);
		if (it == m_messageHistoryMap.end())
		{
			MessageHistory& history = m_messageHistoryMap[messageType];
			history.m_lastInputTime = time;
			history.m_lastRefreshTime = time;
		}
		else
		{
			MessageHistory& history = (*it).second;
			if ( time - history.m_lastInputTime > pkControlRecord->m_frequency)
			{
				history.m_lastInputTime = time;
			}
			else
			{
				++history.m_errorNumber;
			}
			if (time - history.m_lastRefreshTime > pkControlRecord->m_refresh)
			{
				history.m_lastInputTime = time;
				history.m_lastRefreshTime = time;
				history.m_errorNumber = 0;
			}
		}
	}
}

NW_InputMessageRecord::MessageHistory::MessageHistory()
:m_lastInputTime(0)
,m_lastRefreshTime(0)
,m_errorNumber(0)
{

}