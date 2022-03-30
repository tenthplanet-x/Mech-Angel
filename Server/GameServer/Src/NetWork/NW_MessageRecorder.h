#pragma once

#include "SystemInclude.h"
#include <map>
#include "gameplatform_types.h"
#include <wtypes.h>
class NW_MessageRecorder
{
public:
	NW_MessageRecorder(void);
	~NW_MessageRecorder(void);

	void AddMessage(int32_t messageType,char *pMsg, WORD wSize);
	void DiscardMessage(int32_t messageType,char *pMsg, WORD wSize);
//	void AddMessage(int messageID,size_t messageSize);

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
	}
	void Output(size_t playerCount);
	void Clear();

	bool CanSend(int32_t messageType,WORD wSize);
	bool SendFinished(int32_t messageType,WORD wSize);
private:
	struct MessageCount
	{
		MessageCount();

		inline bool operator < (const MessageCount& o) const
		{
			return m_count < o.m_count;
		}
		int32_t m_type;
		size_t m_count;
		size_t m_totalSize;
	};
	typedef hash_map<int32_t,MessageCount> MessageCountMap;
	typedef std::list<MessageCount> MessageCountList;

	MessageCountMap m_messageCounter;
	MessageCountMap m_discardMessageCounter;
	int64_t m_beginTime;

	class MessagebpsControlor
	{
	public:
		MessagebpsControlor();
		bool CanSend(WORD wSize);
		int m_BPS;
	};
	typedef std::vector<MessagebpsControlor> MessagebpsControlorVector;
// 	MessagebpsControlorVector m_bpsControlor;

};
