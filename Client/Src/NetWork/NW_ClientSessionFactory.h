#ifndef _NW_CLIENT_SESSION_FACTORY_H
#define _NW_CLIENT_SESSION_FACTORY_H



class NW_Login2ClientSession;
class NW_Game2ClientSession;
class NW_ClientSessionFactory : public NetWork::NW_NetSessionFactoryBase
{
public:
	NW_ClientSessionFactory();
	virtual ~NW_ClientSessionFactory();

	virtual NetWork::NW_NetSessionBase*		AllocSession(NetSessionType eSessionType);
	virtual void							FreeSession(NetWork::NW_NetSessionBase*	 pkSession );

protected:
	Utility::UT_MemoryPool<NetWork::NW_TempSession>*		m_pkTempSessionPool;
	Utility::UT_MemoryPool<NW_Login2ClientSession>*			m_pkLogin2ClientSessionPool;
	Utility::UT_MemoryPool<NW_Game2ClientSession>*			m_pkGame2ClientSessionPool;
};

#endif