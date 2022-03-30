//接受其他Server的连接，比如调试器
#pragma once

#include "SS_SessionBase.h"

class SS_TempSession : public Session::SS_SessionBase
{
public:
	SS_TempSession();
	~SS_TempSession();

	virtual VOID	OnRecv( BYTE *pMsg, WORD wSize );
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );
};