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
	
	bool InsertMessageToQueue(MessageType eMessageType,MG_MessageBase& rkMessage);		//������Ϣ����
	MG_MessageQueueBase* FindMsgQueue(MessageType eMessageType);		//����ĳ��Ϣ��Ӧ���估����Ϣ������У���Ϊ��Ϣ���䲻���ص������Զ��о���Ψһ�ԣ����Ч���Եͣ�
	bool DispatchMessage(MessageType eMessageType,MG_MessageBase& rkMessage);	//���̷ַ���Ϣ
	bool AttachMessageQueue(MessageType eMinMessageType,
							MessageType eMaxMessageType,
							MessageQueuePriority ePriority,
							MG_MessageQueueBase* pkMessageQueue);		//������Ϣ�������


	
	typedef mem::list<MessageQueueSlot>						MG_MessageQueueSlotList;
	typedef MG_MessageQueueSlotList::iterator				MG_MessageQueueSlotListIter;

private:
	typedef mem::map<MessageQueuePriority,MG_MessageQueueSlotList>  MG_MessageMap;
	typedef MG_MessageMap::iterator									MG_MessageMapIter;

	MG_MessageMap			m_kMessageMap;			//��Ϣ����Ͷ�ݱ�


	typedef mem::map<MessageType,MG_MessageHandlerList>     MG_MessageHandlerMap;
	typedef MG_MessageHandlerMap::iterator					MG_MessageHandlerMapIter;

	MG_MessageHandlerMap    m_kMessageHandlerMap;	//��Ϣhandlerӳ���
};


#endif