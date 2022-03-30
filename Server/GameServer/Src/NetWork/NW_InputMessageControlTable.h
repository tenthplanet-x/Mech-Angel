#pragma once

#include <vector>
#include <map>
#include "gameplatform_types.h"
#include <wtypes.h>
class NW_InputMessageControlTable
{
public:
	NW_InputMessageControlTable(void);
	~NW_InputMessageControlTable(void);

	inline int32_t GetMessageType(char *pMsg, WORD wSize)
	{
		int32_t messageType = 0;
		if (wSize >= 8)
		{
			char intBuf[4] = {0};
			intBuf[3] = * (pMsg+4);
			intBuf[2] = * (pMsg+5);
			intBuf[1] = * (pMsg+6);
			intBuf[0] = * (pMsg+7);

			messageType = *((int32_t*)&intBuf);// + B0 * 0x00ffffff + B1* 0x0000ffff;
		}
		return messageType;
	};

	class InputMessageControlRecord
	{
	public:
		InputMessageControlRecord();
		DWORD	m_frequency;
		int		m_allowErrorConut;
		DWORD	m_refresh;
	};

	const InputMessageControlRecord* GetControlRecord(int32_t messageID)const;


 	typedef std::vector<InputMessageControlRecord> InputMessageControlorVector;
 	InputMessageControlorVector m_inputControlor;
};

class NW_InputMessageRecord
{
public:
	NW_InputMessageRecord();
	void SetControlTable(NW_InputMessageControlTable* controlTable);
	bool CheckRecvMessage(int32_t messageType,DWORD time);
	void SetRecvMessage(int32_t messageType,DWORD time);
	
private:
	class MessageHistory
	{
	public:
		MessageHistory();
		DWORD m_lastInputTime;
		DWORD m_lastRefreshTime;
		int m_errorNumber;
	};

	typedef std::map<int32_t,MessageHistory> InputMessageHistoryMap;
	InputMessageHistoryMap	m_messageHistoryMap;
	NW_InputMessageControlTable* m_controlTable;
};