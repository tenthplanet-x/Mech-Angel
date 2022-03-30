#include "ClientPCH.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "MG_MessageHandlerInterface.h"
using namespace Utility;
//------------------------------------------------------------------
MG_MessageHandlerInterface::MG_MessageHandlerInterface()
: m_bIsActive(true)
{
}
//------------------------------------------------------------------
MG_MessageHandlerInterface::~MG_MessageHandlerInterface()
{
	m_kMessageHandlerList.clear();		
}
//------------------------------------------------------------------
bool MG_MessageHandlerInterface::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerListIter IterBe = m_kMessageHandlerList.begin();
	MG_MessageHandlerListIter IterEd = m_kMessageHandlerList.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		MG_MessageHandlerInterface* pkMessageHandler = (*IterBe);
		if(pkMessageHandler && pkMessageHandler->GetActive())
		{
			pkMessageHandler->RecvMessage(rkMessage);
		}
	}
	return true;
}
//------------------------------------------------------------------
bool MG_MessageHandlerInterface::SendMessage(MG_MessageBase& rkMessage,bool bDispatchImmedia)
{
	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	//T_ASSERT(pkMessageCenter && "NULL Message Center");

	if(pkMessageCenter)
	{
		return pkMessageCenter->SendMessage(rkMessage,bDispatchImmedia);
	}
	else
		return false;
}
//------------------------------------------------------------------
bool MG_MessageHandlerInterface::SendMessage(MG_MessageBase& rkMessage,MG_MessageHandlerInterface* pkReceiver)
{
	T_ASSERT(pkReceiver && "NULL Message pkReceiver");
	if(pkReceiver)
	{
		return pkReceiver->RecvMessage(rkMessage);
	}
	else
		return false;
}
//------------------------------------------------------------------
void MG_MessageHandlerInterface::_attachMessageHandler(MG_MessageHandlerInterface* pkMessageHandler)
{
#ifndef _SHIPPING
	/// 重入检查
	MG_MessageHandlerListIter IterBe = m_kMessageHandlerList.begin();
	MG_MessageHandlerListIter IterEd = m_kMessageHandlerList.end();
	for( ; IterBe != IterEd; ++IterBe)
	{
		MG_MessageHandlerInterface* pkFindMessageHandler = (*IterBe);
		if(pkFindMessageHandler == pkMessageHandler)
		{
			T_ASSERT2(false, "_attachMessageHandler 重入错误");
		}
	}
#endif

	m_kMessageHandlerList.push_back(pkMessageHandler);
}
//------------------------------------------------------------------
void MG_MessageHandlerInterface::_detachMessageHandler(MG_MessageHandlerInterface* pkMessageHandler)
{
	MG_MessageHandlerListIter IterBe = m_kMessageHandlerList.begin();
	MG_MessageHandlerListIter IterEd = m_kMessageHandlerList.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		MG_MessageHandlerInterface* pkFindMessageHandler = (*IterBe);
		if(pkFindMessageHandler == pkMessageHandler)
		{
			m_kMessageHandlerList.erase(IterBe);
			return;
		}
	}

	//T_ASSERT(false && "can't find message handler");
	GfxWriteLog( LOG_WARNING_FILELINE , LOG_SWITCH_WARNING , "Can't find Message Handler %0x" , pkMessageHandler );
}