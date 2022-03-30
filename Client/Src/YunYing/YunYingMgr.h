#ifndef _CLIENT_YUNYING_MANAGER_H
#define _CLIENT_YUNYING_MANAGER_H 

#include "MG_MessageHandlerInterface.h"
#include "MM_BaseObject.h"
#include "YunYingPlatform.h"

/*************************************************************************
    对接平台相关
*************************************************************************/

class	LC_YunYingManager : public Memory::MM_BaseObject,
							public MG_MessageHandlerInterface
{
public:
	LC_YunYingManager();
	~LC_YunYingManager();

	//////////////////////////////////////////////////////////////////////////

	virtual bool RecvMessage(MG_MessageBase& rkMessage); 

private:
	void		_event_TimerTick(MG_MessageBase& rkMessage);
	void		_event_NewRole(MG_MessageBase& rkMessage);
	void		_event_GamePlayerLogin(MG_MessageBase& rkMessage);
	void		_event_GamePlayerLevelUp(MG_MessageBase& rkMessage);
	void		_event_GamePlayerQuit(MG_MessageBase& rkMessage);
	void		_event_GamePlayerConsume(MG_MessageBase& rkMessage);
};

#endif
