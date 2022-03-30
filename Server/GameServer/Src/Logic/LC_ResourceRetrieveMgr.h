#ifndef _LC_RESOURCE_RETRIEVE_MGR_H_
#define _LC_RESOURCE_RETRIEVE_MGR_H_
#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"
#include "IdDefine.h"
#include "UT_ComplexData.h"
#include "SystemError.h"
#include "char_data.pb.h"

class LC_ServerPlayer;
namespace GameLogic
{
	class LC_ResourceRetrieveMgr
	{
	public:
		LC_ResourceRetrieveMgr();

		~LC_ResourceRetrieveMgr();

		void Save(char_data_define::char_ResourceRetrieve& info);
		void Load(const char_data_define::char_ResourceRetrieve& info);

		void Init(object_id_type nOwnerPlayerId);	
		void ReUse();
		LC_ServerPlayer* GetOwnerPlayer();

		ResultType ActiveState();
		bool IsActive();

		ResultType GetResourceRetrieverReward(int id, bool specialFind);
		void GetResourceRetrieverRewardList(StringType& reward, int index, Utility::UT_SIMDataList& itemList);
		int GetVaildTimes(int id);
		void SetVaildTimes(int id, int times);
		void SyncResourceRetrieverRecord();

		void UpdateResourceRetrieveRecord();

		uint32_t GetTimeStamp();
		void SetTimeStamp(uint32_t time);

		void SyncState();
	private:
		bool m_nActivateState;
		object_id_type m_nOwnerPlayerId;
		uint32_t m_nTimeStamp;
		mem::map<int, int> m_nResourceRetrieveRecord;

		int32_t m_nRewardPlayerLevel;
		mem::map<int, int> m_nRewardMapID;
	};
}

#endif