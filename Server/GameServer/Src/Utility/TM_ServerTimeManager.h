//服务器系统定时器
#ifndef _TM_SERVER_TIME_MANAGER_H
#define _TM_SERVER_TIME_MANAGER_H

#include "UT_TimeManager.h"

class TM_ServerTimeManager : public Utility::UT_TimeManager
{
public:
	TM_ServerTimeManager();
	virtual ~TM_ServerTimeManager();
};


#endif