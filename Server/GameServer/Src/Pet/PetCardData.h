#ifndef __PET_CARD_DATA_H__
#define __PET_CARD_DATA_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"

class LC_ServerPlayer;
namespace Pet
{
	class PetCardData //: public GameLogic::LC_ServerPlayer
	{
	public:
		PetCardData(void){}
		~PetCardData();

		void Init(int32_t iID){}
		void Clear(void){}

		//int LoadFromDB();
		//int LoadFromTemplate(int32_t nTemplateId);

		//const PetInstance* Get();
		//int SaveData();
		// DBLogic
	protected:
		friend class PetInstanceData;
	};
}

#endif //__ACT_ACTIVITY_ASSET_H__
