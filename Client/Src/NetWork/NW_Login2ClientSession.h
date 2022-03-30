#ifndef _NW_LOGIN_2_CLIENT_SESSION_H
#define _NW_LOGIN_2_CLIENT_SESSION_H
//客户端Session，模拟器环境下，可创建多个
#include "NW_Game2ClientSession.h"

class LC_ClientGamePlayer;
class NW_Login2ClientSession : public NW_Game2ClientSession//NetWork::NW_NetSessionBase
{
public:
	NW_Login2ClientSession();
	virtual ~NW_Login2ClientSession();

    virtual void	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );
	virtual void	OnRecv( char *pMsg, uint16_t wSize );
    virtual void    OnDisconnect();
	virtual void	KeepAlive(float fCurrentTime);

	void			FakeAuth();
};

#endif