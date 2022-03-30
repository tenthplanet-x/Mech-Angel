//服务器端CSV管理类
#ifndef _CF_SERVER_CSV_MANAGER_H
#define _CF_SERVER_CSV_MANAGER_H

#include "MM_BaseObject.h"
#include "UT_Singleton.h"

class CF_ServerCSVManager : public Memory::MM_BaseObject,
	public Utility::UT_Singleton<CF_ServerCSVManager>
{
public:
	CF_ServerCSVManager(void);
	virtual ~CF_ServerCSVManager(void);

	bool Init(void);
	void UnInit(void);

private:
};

#endif