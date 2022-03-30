#ifndef _LC_VIP_MANAGER_H
#define _LC_VIP_MANAGER_H
#include "MM_BaseObject.h"
#include "UT_Singleton.h"

class LC_ServerPlayer;
class LC_VIPManager : public Memory::MM_BaseObject,
						public Utility::UT_Singleton<LC_VIPManager>
{
public:
	LC_VIPManager();
	~LC_VIPManager();

	void	Init();
	void	UnInit();
	void	Update(float fCurrentTime);
		
	void	PlayerEnterGame(LC_ServerPlayer* pkPlayer,uint32_t uiIP);	
	void	PlayerKillNPC(LC_ServerPlayer* pkPlayer,int lNPCTypeID);
	void	PlayerSendGMCommand(LC_ServerPlayer* pkPlayer,const StringType& rkCommand );
protected:
	mem::vector<uint32_t>		m_kIPList;
	float						m_fLastUpdateTime;
};

#endif