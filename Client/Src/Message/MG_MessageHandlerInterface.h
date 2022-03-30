#ifndef _MG_MESSAGE_HANDLER_H
#define _MG_MESSAGE_HANDLER_H


class MG_MessageBase;
class MG_MessageHandlerInterface 
{
public:
	MG_MessageHandlerInterface();
	virtual ~MG_MessageHandlerInterface();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);
	virtual bool SendMessage(MG_MessageBase& rkMessage,bool bDispatchImmedia = false);
	virtual bool SendMessage(MG_MessageBase& rkMessage,MG_MessageHandlerInterface* pkReceiver);

	virtual void SetActive(bool bActive) { m_bIsActive = bActive; }
	virtual bool GetActive() { return m_bIsActive; }
protected:
	bool		 m_bIsActive;
	void	_attachMessageHandler(MG_MessageHandlerInterface* pkMessageHandler);
	void    _detachMessageHandler(MG_MessageHandlerInterface* pkMessageHandler);

	typedef mem::list<MG_MessageHandlerInterface*>		MG_MessageHandlerList;	//MG_MessageHandlerInterface包含自身类型的指针列表，构成一棵handler树
	typedef MG_MessageHandlerList::iterator		MG_MessageHandlerListIter;
	MG_MessageHandlerList						m_kMessageHandlerList;
};

#define BEGIN_MESSAGE_HANDLE switch(rkMessage.m_eMsgType) {
#define MESSAGE_HANDLE(msgType,func) case msgType : func(rkMessage);break;
#define END_MESSAGE_HANDLE   default: break; }


typedef mem::list<MG_MessageHandlerInterface*>   MG_MessageHandlerList;
typedef MG_MessageHandlerList::iterator MG_MessageHandlerListIter;

#endif