#ifndef __BUDDY_CARD_DATA_H__
#define __BUDDY_CARD_DATA_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"

class LC_ServerPlayer;
namespace Buddy
{
	class BuddyCardData //: public GameLogic::LC_ServerPlayer
	{
	public:
		BuddyCardData(void){}
		~BuddyCardData();

		void Init(int32_t iID){}
		void Clear(void){}

		//int LoadFromDB();
		//int LoadFromTemplate(int32_t nTemplateId);

		//const BuddyInstance* Get();
		//int SaveData();
		// DBLogic
	protected:
		friend class BuddyInstanceData;
	};
}

#endif //__ACT_ACTIVITY_ASSET_H__
