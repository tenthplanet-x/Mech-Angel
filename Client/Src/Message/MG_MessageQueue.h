#ifndef _MG_MESSAGE_QUEUE_H
#define _MG_MESSAGE_QUEUE_H

#include "MG_MessageDefine.h"

class MG_MessageQueueBase : public Memory::MM_BaseObject
{
public:
	MG_MessageQueueBase();
	virtual ~MG_MessageQueueBase();

	virtual bool PushMessage(MG_MessageBase& rkMessage) = 0;
	virtual bool PopMessage(MG_MessageBase& rkMessage) = 0;
	
	virtual bool Update(float fTime) = 0;
	virtual bool Clear() = 0;
	virtual bool Empty() = 0;

	void	SetMessageLifeTime(float fLifeTime);
	float   GetMessageLifeTime();
protected:
	float   m_fMessageLifeTime;
};

//标准消息队列，消息永不过期
class MG_MessageNeverDiedQueue : public MG_MessageQueueBase
{
public:
	MG_MessageNeverDiedQueue();
	virtual ~MG_MessageNeverDiedQueue();

	virtual bool PushMessage(MG_MessageBase& rkMessage);
	virtual bool PopMessage(MG_MessageBase& rkMessage);

	virtual bool Update(float fTime);
	virtual bool Clear();
	virtual bool Empty();

protected:
	MG_MessageList m_kMessageList;
};

#endif