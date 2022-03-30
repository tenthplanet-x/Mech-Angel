#include "ClientPCH.h"
#include "MG_MessageBase.h"
#include "MG_MessageQueue.h"
//-------------------------------------------------------
MG_MessageQueueBase::MG_MessageQueueBase()
: m_fMessageLifeTime(-1.0f) //设置为负数关闭过期清理功能
{
}
//-------------------------------------------------------
MG_MessageQueueBase::~MG_MessageQueueBase()
{
}
//-------------------------------------------------------
void MG_MessageQueueBase::SetMessageLifeTime(float fLifeTime)
{
	m_fMessageLifeTime = fLifeTime;
}
//-------------------------------------------------------
float MG_MessageQueueBase::GetMessageLifeTime()
{
	return m_fMessageLifeTime;
}
//-------------------------------------------------------
//-------------------------------------------------------
MG_MessageNeverDiedQueue::MG_MessageNeverDiedQueue()
: MG_MessageQueueBase()
{
	m_kMessageList.clear();
}
//-------------------------------------------------------
MG_MessageNeverDiedQueue::~MG_MessageNeverDiedQueue()
{
	m_kMessageList.clear();
}
//-------------------------------------------------------
bool MG_MessageNeverDiedQueue::PushMessage(MG_MessageBase& rkMessage)
{
	m_kMessageList.push_back(rkMessage);
	return true;
}
//-------------------------------------------------------
bool MG_MessageNeverDiedQueue::PopMessage(MG_MessageBase& rkMessage)
{
	if(m_kMessageList.size() > 0)
	{
		rkMessage = m_kMessageList.front();
		m_kMessageList.pop_front();
		return true;
	}	
	return false;
}
//-------------------------------------------------------
bool MG_MessageNeverDiedQueue::Update(float fTime)
{
	return true;
}
//-------------------------------------------------------
bool MG_MessageNeverDiedQueue::Clear()
{
	m_kMessageList.clear();
	return true;
}
//-------------------------------------------------------
bool MG_MessageNeverDiedQueue::Empty()
{
	return (m_kMessageList.size() == 0);
}