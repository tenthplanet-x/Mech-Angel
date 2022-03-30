//客户端获取状态机控制器的工厂类

#ifndef _SM_GAMESERVER_CONTROLLER_FACTORY_H
#define _SM_GAMESERVER_CONTROLLER_FACTORY_H

#include "SM_ControllerFactory.h"

namespace StateMachine
{
	class SM_ControllerBase;
	class SM_ServerControllerFactory : public SM_ControllerFactory
	{
	public:
		SM_ServerControllerFactory();
		~SM_ServerControllerFactory();

		virtual bool Init();
	};
}

#endif