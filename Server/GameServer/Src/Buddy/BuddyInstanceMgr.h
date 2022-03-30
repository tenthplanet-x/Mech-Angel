#ifndef __BUDDY_INSTANCE_MGR_H__
#define __BUDDY_INSTANCE_MGR_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"

class LC_ServerPlayer;
namespace Buddy
{
	class BuddyInstance;
	class BuddyInstanceMgr
	{
	public:
		BuddyInstanceMgr(void)
		{}
		~BuddyInstanceMgr();

		void Init(int32_t iID){}
		void Clear(void){}

		// GameLogic
		int GetActivateCount();
		int GetUsageCount();
		// DBLogic
	protected:
	};
}

#endif //__ACT_ACTIVITY_ASSET_H__
