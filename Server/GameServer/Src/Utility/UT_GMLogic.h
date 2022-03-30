#ifndef __UT_GM_LOGIC_H__
#define __UT_GM_LOGIC_H__

#pragma once

#include "redis.pb.h"
#include "UT_Redis.h"
#include "UT_RedisRequest.h"
#include "UT_Singleton.h"

#include <string>
#include <queue>
#include <map>

#include "simple_thread.h"

#include <boost/atomic.hpp>

#include "SystemError.h"
#include "ajson.hpp"


namespace Utility
{
	class UT_GMLogic: public Utility::UT_Singleton<UT_GMLogic>
	{
	public:
		UT_GMLogic();
		virtual ~UT_GMLogic();
	public:
		void Init();
		void UnInit();

		void Update( float fCurrent, float fDelta );
	private:
		void Update_SendMail(float fCurrent, float fDelta);
		void Update_BroadcastMsg(float fCurrent, float fDelta);
		void Update_KickUser(float fCurrent, float fDelta);
	};
}

//AJSON(Utility::channel_msg, (type)(id))
//AJSON(Utility::gm_cmd_resp, (type)(channel)(cmd)(code)(msg)(data))
//AJSON(Utility::gm_cmd_infosort, (serverid)(key))
#endif
