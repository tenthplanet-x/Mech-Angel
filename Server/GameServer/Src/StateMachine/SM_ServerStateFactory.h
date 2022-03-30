//服务器专用状态元创建工厂
#ifndef _SM_SERVER_STATE_FACTORY_H
#define _SM_SERVER_STATE_FACTORY_H

#include "SystemDefine.h"
#include "StringDefine.h"
#include "SM_StateFactory.h"

class SM_ServerStateFactory : public StateMachine::SM_StateFactory
{
public:
	SM_ServerStateFactory();
	virtual ~SM_ServerStateFactory();

	virtual bool Init();

	const StringType&	GetScriptStateName(int iStateID);

private:

	bool _InitScriptFile();
	bool _InitScriptState(const StringType &rkScriptName);

	StateMachine::SM_StateBase* _createState(int iStateID);
};

#endif
