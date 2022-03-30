#include "ClientPCH.h"
#include "MG_MessageCenter.h"
#include "MG_MessageBase.h"
#include "MG_MessageQueue.h"
#include "UT_Profile.h"

//---------------------------------------------------------------------
MG_MessageCenter::MG_MessageCenter()
{
	m_kMessageMap.clear();
	m_kMessageHandlerMap.clear();

	MG_MessageNeverDiedQueue* pkDefaultMessageQueue = T_NEW_D MG_MessageNeverDiedQueue;
	AttachMessageQueue(MGT_INVALID,
					   MGT_MAXCOUNT,
					   MQP_HIGH,
					   pkDefaultMessageQueue);
}
//---------------------------------------------------------------------
MG_MessageCenter::~MG_MessageCenter()
{
	MG_MessageMapIter IterMessageMapBe = m_kMessageMap.begin();
	MG_MessageMapIter IterMessageMapEd = m_kMessageMap.end();
	MG_MessageQueueSlotListIter IterSlotQueueBe,IterSlotQueueEd;

	
	for( ; IterMessageMapBe != IterMessageMapEd; ++IterMessageMapBe)
	{
		MG_MessageQueueSlotList& rkQueueSlotList = (*IterMessageMapBe).second;
		IterSlotQueueBe = rkQueueSlotList.begin();
		IterSlotQueueEd = rkQueueSlotList.end();
		for( ; IterSlotQueueBe != IterSlotQueueEd; ++IterSlotQueueBe)
		{
			MessageQueueSlot& rkMessageQueueSlot = (*IterSlotQueueBe);
			MG_MessageQueueBase* pkMessageQueue = rkMessageQueueSlot.m_pkMessageQueue;
			T_ASSERT(pkMessageQueue && "Null Message Queue");
			T_SAFE_DELETE(pkMessageQueue );
		}
	}

 
	m_kMessageMap.clear();

	//不释放Handler,由外部系统释放
	m_kMessageHandlerMap.clear();
}
//---------------------------------------------------------------------
bool MG_MessageCenter::SendMessage(MG_MessageBase& rkMessage,bool bDispatchImmediately /* = false */)
{
//	T_ASSERT(GfxIsMainThread());
	MessageType eMessageType = rkMessage.m_eMsgType;
	
	T_ASSERT( CHECK_MESSAGE_VALID(eMessageType)  && "You have send a invalid message");
	
	if(!CHECK_MESSAGE_VALID(eMessageType))
	{
		return false;
	}
	bool bResult = false;
	if(bDispatchImmediately)
	{
		bResult = DispatchMessage(eMessageType,rkMessage);
	}
	else
	{
		bResult = InsertMessageToQueue(eMessageType,rkMessage);
	}

	return bResult;
}
//---------------------------------------------------------------------
void MG_MessageCenter::Update(float fTime)
{	
	T_ASSERT(GfxIsMainThread());
	MG_MessageMapIter IterMessageMapBe = m_kMessageMap.begin();
	MG_MessageMapIter IterMessageMapEd = m_kMessageMap.end();
	MG_MessageQueueSlotListIter IterSlotQueueBe,IterSlotQueueEd;
	for( ; IterMessageMapBe != IterMessageMapEd; ++IterMessageMapBe)
	{
		T_PROFILE("MessageQueue::Update");
		MG_MessageQueueSlotList& rkQueueSlotList = (*IterMessageMapBe).second;
		IterSlotQueueBe = rkQueueSlotList.begin();
		IterSlotQueueEd = rkQueueSlotList.end();
		for( ; IterSlotQueueBe != IterSlotQueueEd; ++IterSlotQueueBe)
		{
			MessageQueueSlot& rkMessageQueueSlot = (*IterSlotQueueBe);
			MG_MessageQueueBase* pkMessageQueue = rkMessageQueueSlot.m_pkMessageQueue;
			T_ASSERT(pkMessageQueue && "Null Message Queue");
			if(pkMessageQueue)
			{
				pkMessageQueue->Update(fTime);
			}
		}
	}


	IterMessageMapBe = m_kMessageMap.begin();
	MessageType eMessageType;

	for( ; IterMessageMapBe != IterMessageMapEd; ++IterMessageMapBe)
	{
		MG_MessageQueueSlotList& rkQueueSlotList = (*IterMessageMapBe).second;
		IterSlotQueueBe = rkQueueSlotList.begin();
		IterSlotQueueEd = rkQueueSlotList.end();
		for( ; IterSlotQueueBe != IterSlotQueueEd; ++IterSlotQueueBe)
		{
			MessageQueueSlot& rkMessageQueueSlot = (*IterSlotQueueBe);
			MG_MessageQueueBase* pkMessageQueue = rkMessageQueueSlot.m_pkMessageQueue;
			T_ASSERT(pkMessageQueue && "Null Message Queue");
			if(pkMessageQueue)
			{
				while(!pkMessageQueue->Empty())
				{
					MG_MessageBase kMessage;
					pkMessageQueue->PopMessage(kMessage);
					eMessageType = kMessage.m_eMsgType;
					T_ASSERT( CHECK_MESSAGE_VALID(eMessageType)  && "You have send a invalid message");
					DispatchMessage(eMessageType,kMessage);
				}
			}
		}
	}
}
//---------------------------------------------------------------------
bool MG_MessageCenter::RegisterMessageHandler(MessageType eMessageType,MG_MessageHandlerInterface* pkMessageHandler)
{
	T_ASSERT( CHECK_MESSAGE_VALID(eMessageType) && "You have register a invalid message handler");
	T_ASSERT_CRITICAL_S(pkMessageHandler);

	if(!CHECK_MESSAGE_VALID(eMessageType) || !pkMessageHandler)
    {
		GfxWriteLog(LOG_WARNING_FILELINE, Utility::LOG_SWITCH_WARNING, "You have register a invalid message handler");
        return false;
    }

	MG_MessageHandlerMapIter IterFind = m_kMessageHandlerMap.find(eMessageType);
	if(IterFind == m_kMessageHandlerMap.end())
	{
		MG_MessageHandlerList kHandlerList;
		kHandlerList.clear();
		kHandlerList.push_back(pkMessageHandler);
		m_kMessageHandlerMap[eMessageType] = kHandlerList;
	}
	else
	{
		MG_MessageHandlerList& rkInsertList = (*IterFind).second;
		rkInsertList.push_back(pkMessageHandler);
	}

	return true;
}
//---------------------------------------------------------------------
bool MG_MessageCenter::InsertMessageToQueue(MessageType eMessageType,MG_MessageBase& rkMessage)
{
	MG_MessageQueueBase* pkQueueBase = FindMsgQueue(eMessageType);
	if(!pkQueueBase)
		return false;

	pkQueueBase->PushMessage(rkMessage);
	return true;
}
//---------------------------------------------------------------------
bool MG_MessageCenter::DispatchMessage(MessageType eMessageType,MG_MessageBase& rkMessage)
{
	MG_MessageHandlerMapIter IterFind = m_kMessageHandlerMap.find(eMessageType);
	if(IterFind == m_kMessageHandlerMap.end())
		return false;

	MG_MessageHandlerList& rkMessageHandlerList = (*IterFind).second;
	MG_MessageHandlerListIter IterBe = rkMessageHandlerList.begin();
	MG_MessageHandlerListIter IterEd = rkMessageHandlerList.end();

	for( ; IterBe != IterEd; ++IterBe )
	{
		MG_MessageHandlerInterface* pkMessageHandler = (*IterBe);
		T_ASSERT(pkMessageHandler && "NULL Message Handler");
		if(pkMessageHandler)
			pkMessageHandler->RecvMessage(rkMessage);
	}
	return true;
}
//---------------------------------------------------------------------
bool MG_MessageCenter::AttachMessageQueue(MessageType eMinMessageType, 
											MessageType eMaxMessageType, 
											MessageQueuePriority ePriority, 
											MG_MessageQueueBase* pkMessageQueue)
{
	T_ASSERT(pkMessageQueue && "NULL Message Handler");
	if(!pkMessageQueue)
		return false;

	MG_MessageMapIter IterMessageMapBe = m_kMessageMap.begin();
	MG_MessageMapIter IterMessageMapEd = m_kMessageMap.end();
	MG_MessageQueueSlotListIter IterSlotQueueBe,IterSlotQueueEd;
	MessageType eSlotMinMessageType,eSlotMaxMessageType;

	for( ; IterMessageMapBe != IterMessageMapEd; ++IterMessageMapBe)
	{
		MG_MessageQueueSlotList& rkQueueSlotList = (*IterMessageMapBe).second;
		IterSlotQueueBe = rkQueueSlotList.begin();
		IterSlotQueueEd = rkQueueSlotList.end();
		for( ; IterSlotQueueBe != IterSlotQueueEd; ++IterSlotQueueBe)
		{
			MessageQueueSlot& rkMessageQueueSlot = (*IterSlotQueueBe);
			eSlotMinMessageType = rkMessageQueueSlot.m_eMinMessageType;
			eSlotMaxMessageType = rkMessageQueueSlot.m_eMaxMessageType;
			if( (eMinMessageType >= eSlotMinMessageType && eMinMessageType <= eSlotMaxMessageType ) ||		//msg区间有重叠，不允许
				(eMaxMessageType >= eSlotMinMessageType && eMaxMessageType <= eSlotMaxMessageType ) )
			{
				T_ASSERT(false && "Conflict Message Queue");		
				return false;
			}
		}
	}

	MessageQueueSlot kQueueSlot;
	kQueueSlot.m_eMinMessageType = eMinMessageType;
	kQueueSlot.m_eMaxMessageType = eMaxMessageType;
	kQueueSlot.m_pkMessageQueue = pkMessageQueue;

	MG_MessageMapIter IterFind = m_kMessageMap.find(ePriority);
	if(IterFind == m_kMessageMap.end())	//该ePriority类型的QueueSlotList没有，新加一个
	{
		MG_MessageQueueSlotList kQueueSlotList;
		kQueueSlotList.clear();
		kQueueSlotList.push_back(kQueueSlot);

		m_kMessageMap[ePriority] = kQueueSlotList;
	}
	else
	{
		MG_MessageQueueSlotList& rkQueueSlotList = (*IterFind).second;
		rkQueueSlotList.push_back(kQueueSlot);
	}

	return true;
}
//-------------------------------------------------------------------------------
MG_MessageQueueBase* MG_MessageCenter::FindMsgQueue( MessageType eMessageType )
{
	MG_MessageMapIter IterMessageMapBe = m_kMessageMap.begin();
	MG_MessageMapIter IterMessageMapEd = m_kMessageMap.end();
	MG_MessageQueueSlotListIter IterSlotQueueBe,IterSlotQueueEd;
	MessageType eSlotMinMessageType,eSlotMaxMessageType;

	for( ; IterMessageMapBe != IterMessageMapEd; ++IterMessageMapBe)
	{
		MG_MessageQueueSlotList& rkQueueSlotList = (*IterMessageMapBe).second;
		IterSlotQueueBe = rkQueueSlotList.begin();
		IterSlotQueueEd = rkQueueSlotList.end();
		for( ; IterSlotQueueBe != IterSlotQueueEd; ++IterSlotQueueBe)
		{
			MessageQueueSlot& rkMessageQueueSlot = (*IterSlotQueueBe);
			MG_MessageQueueBase* pkMessageQueue = rkMessageQueueSlot.m_pkMessageQueue;
			T_ASSERT(pkMessageQueue && "Null Message Queue");
			if(pkMessageQueue)
			{
				eSlotMinMessageType = rkMessageQueueSlot.m_eMinMessageType;
				eSlotMaxMessageType = rkMessageQueueSlot.m_eMaxMessageType;
				if(eMessageType >= eSlotMinMessageType && eMessageType <= eSlotMaxMessageType)
					return pkMessageQueue;
			}
		}
	}

	return NULL;
}