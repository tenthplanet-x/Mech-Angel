#include "ClientPCH.h"

#include "NW_ClientSessionFactory.h"
#include "NW_Login2ClientSession.h"
#include "NW_Game2ClientSession.h"

using namespace NetWork;
using namespace Utility;
//-------------------------------------------------
NW_ClientSessionFactory::NW_ClientSessionFactory()
: NW_NetSessionFactoryBase()
{
	m_pkTempSessionPool				= T_NEW_D UT_MemoryPool<NW_TempSession>;
	m_pkLogin2ClientSessionPool		= T_NEW_D UT_MemoryPool<NW_Login2ClientSession>;
	m_pkGame2ClientSessionPool		= T_NEW_D UT_MemoryPool<NW_Game2ClientSession>;


	m_pkTempSessionPool->Init(2 , 10);
	m_pkLogin2ClientSessionPool->Init(1, 10);
    m_pkGame2ClientSessionPool->Init(1, 10);
}
//-------------------------------------------------
NW_ClientSessionFactory::~NW_ClientSessionFactory()
{
	T_SAFE_DELETE(m_pkTempSessionPool);
	T_SAFE_DELETE(m_pkLogin2ClientSessionPool);
	T_SAFE_DELETE(m_pkGame2ClientSessionPool);
}
//-------------------------------------------------
NW_NetSessionBase* NW_ClientSessionFactory::AllocSession(NetSessionType eSessionType)
{
	NW_NetSessionBase* pkResult = NULL;
	switch (eSessionType)
	{
    case NST_TEMP:
		pkResult = (NW_NetSessionBase*)m_pkTempSessionPool->Alloc();
		break;
	case NST_CLIENT_TO_LOGINSERVER:
		pkResult = (NW_NetSessionBase*)m_pkLogin2ClientSessionPool->Alloc();
		break;
	case NST_CLIENT_TO_GAMESERVER:
		pkResult = (NW_NetSessionBase*)m_pkGame2ClientSessionPool->Alloc();
		break;
	default:
		T_ASSERT(false);
		break;
	}

	return pkResult;
}
//-------------------------------------------------
void NW_ClientSessionFactory::FreeSession(NW_NetSessionBase* pkSession )
{
	switch (pkSession->GetSessionType())
	{
	case NST_TEMP:
		m_pkTempSessionPool->Free( ( NW_TempSession* )pkSession );
		break;
	case NST_CLIENT_TO_LOGINSERVER:
		m_pkLogin2ClientSessionPool->Free((NW_Login2ClientSession*)pkSession);
		break;
	case NST_CLIENT_TO_GAMESERVER:
		m_pkGame2ClientSessionPool->Free( (NW_Game2ClientSession*)pkSession );
		break;
	default:

		T_ASSERT(false);
		break;
	}
}