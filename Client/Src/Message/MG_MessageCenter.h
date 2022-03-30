#ifndef _MG_MESSAGE_CENTER_H
#define _MG_MESSAGE_CENTER_H

#include "MG_MessageDefine.h"
#include "MG_MessageHandlerInterface.h"

class MG_MessageBase;
class MG_MessageQueueBase;

class MG_MessageCenter : public Utility::UT_Singleton<MG_MessageCenter>,
						 public Memory::MM_BaseObject
{
public:
	MG_MessageCenter();
	~MG_MessageCenter();

	bool SendMessage(MG_MessageBase& rkMessage,bool bDispatchImmediately = false);
	void Update(float fTime);
	bool RegisterMessageHandler(MessageType eMessageType,MG_MessageHandlerInterface* pkMessageHandler);

protected:
	enum MessageQueuePriority
	{	
		MQP_HIGH,
		MQP_NORMAL,
		MQP_LOW,
		MQP_MAXCOUNT,
	};

	struct MessageQueueSlot
	{
		MessageType m_eMinMessageType;
		MessageType m_eMaxMessageType;
		MessageQueuePriority m_eQueuePriority;
		MG_MessageQueueBase* m_pkMessageQueue;
	};
	
	bool InsertMessageToQueue(MessageType eMessageType,MG_MessageBase& rkMessage);		//插入消息对象
	MG_MessageQueueBase* FindMsgQueue(MessageType eMessageType);		//查找某消息对应区间及其消息对象队列，因为消息区间不会重叠，所以队列具有唯一性（这块效率略低）
	bool DispatchMessage(MessageType eMessageType,MG_MessageBase& rkMessage);	//立刻分发消息
	bool AttachMessageQueue(MessageType eMinMessageType,
							MessageType eMaxMessageType,
							MessageQueuePriority ePriority,
							MG_MessageQueueBase* pkMessageQueue);		//新增消息对象队列


	
	typedef mem::list<MessageQueueSlot>						MG_MessageQueueSlotList;
	typedef MG_MessageQueueSlotList::iterator				MG_MessageQueueSlotListIter;

private:
	typedef mem::map<MessageQueuePriority,MG_MessageQueueSlotList>  MG_MessageMap;
	typedef MG_MessageMap::iterator									MG_MessageMapIter;

	MG_MessageMap			m_kMessageMap;			//消息对象投递表


	typedef mem::map<MessageType,MG_MessageHandlerList>     MG_MessageHandlerMap;
	typedef MG_MessageHandlerMap::iterator					MG_MessageHandlerMapIter;

	MG_MessageHandlerMap    m_kMessageHandlerMap;	//消息handler映射表
};


#endif