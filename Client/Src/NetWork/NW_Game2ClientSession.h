#ifndef _NW_GAME_2_CLIENT_SESSION_H
#define _NW_GAME_2_CLIENT_SESSION_H 
//客户端Session，模拟器环境下，可创建多个


class LC_ClientGamePlayer;
class NW_Game2ClientSession : public NetWork::NW_NetSessionBase
{
public:
	NW_Game2ClientSession();
	virtual ~NW_Game2ClientSession();

	virtual void OnRecv( char* pMsg, uint16_t wSize );
	virtual void OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );
    virtual void OnDisconnect();

    void		 SetAutoReconnect(bool bAutoReconnect = true);
	void		 SetPlayer(LC_ClientGamePlayer* pkPlayer);
	LC_ClientGamePlayer* GetPlayer();

protected:
    bool                    m_bAutoReconnect;
	LC_ClientGamePlayer*	m_pkPlayer;
};

#endif