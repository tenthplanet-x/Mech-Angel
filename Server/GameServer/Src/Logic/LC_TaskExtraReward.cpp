#include "LC_TaskExtraReward.h"
#include "LC_ServerPlayer.h"

#include "MG_GameProtocol.h"
#include "MG_SyncProtocol.h"
#include "MG_TYPES.h"

CTaskExtraReward::CTaskExtraReward()
{
	m_ExtraRewardData.clear();
}

CTaskExtraReward::~CTaskExtraReward()
{

}

void CTaskExtraReward::SaveData(char_data_define::PBTaskExtraReward* pkInfo)
{
	if(NULL == pkInfo)
	{
		return;
	}

	for(mem::map<int32_t, mem::map<int32_t, int32_t> > ::iterator it=m_ExtraRewardData.begin(); it!=m_ExtraRewardData.end(); ++it)
	{
		char_data_define::PBItemRewardInfo* pkData = pkInfo->add_itemdata();
		if(NULL != pkData)
		{
			pkData->set_itembindmode(it->first);
			for(mem::map<int32_t, int32_t>::iterator kIt=it->second.begin(); kIt!=it->second.end(); ++kIt)
			{
				::char_data_define::PBKeyValue* pkInfo = pkData->add_itemdata();
				if(NULL != pkInfo)
				{
					pkInfo->set_key(kIt->first);
					pkInfo->set_value(kIt->second);
				}
			}
		}
	}
}

void CTaskExtraReward::LoadData(const char_data_define::PBTaskExtraReward& info)
{
	m_ExtraRewardData.clear();
	for(int i=0; i<info.itemdata_size(); ++i)
	{
		int32_t bindMode = info.itemdata(i).itembindmode();
		mem::map<int32_t, int32_t> data;
		for(int j=0; j<info.itemdata(i).itemdata_size(); ++j)
		{
			data[info.itemdata(i).itemdata(j).key()] = info.itemdata(i).itemdata(j).value();
		}

		m_ExtraRewardData.insert(std::make_pair(bindMode, data));
	}
}

void CTaskExtraReward::SyncTaskExtraReward(LC_ServerPlayer* pkPlayer)
{
	if(NULL==pkPlayer || !pkPlayer->IsMainController())
	{
		return;
	}

	MG_SyncTaskExtraReward syncMsg;
	syncMsg.m_nSyncType = SyncType_All;
	syncMsg.m_nSyncOP = SyncOP_Add;
	syncMsg.m_nRewardInfo.reserve(m_ExtraRewardData.size());
	for(mem::map<int32_t, mem::map<int32_t, int32_t> >::iterator it=m_ExtraRewardData.begin(); it!=m_ExtraRewardData.end(); ++it)
	{
		for(mem::map<int32_t, int32_t>::iterator kIt=it->second.begin(); kIt!=it->second.end(); ++kIt)
		{
			KeyValue_Base data;
			data.key =kIt->first;
			data.value = kIt->second;
			syncMsg.m_nRewardInfo.push_back(data);
		}
	}
	pkPlayer->SendMsgToClient(MGPT_SYNCTASKEXTRAREWARD, &syncMsg);
}

void CTaskExtraReward::GetTaskExtraReward(LC_ServerPlayer* pkPlayer)
{
	if(NULL==pkPlayer || !pkPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_GetTaskExtraReward rltMsg;
	rltMsg.m_nResult = RE_SUCCESS;
	uint32_t res = LC_Helper::GetVipPrivilegeTimesByType(VIP_PC_TaskExtraReward, pkPlayer);
	if(res <= 0)
	{
		rltMsg.m_nResult = RE_GET_TASK_EXTRAREWARD_ERROR_1;
	}
	else if(m_ExtraRewardData.empty())
	{
		rltMsg.m_nResult = RE_GET_TASK_EXTRAREWARD_ERROR_2;
	}

	if(RE_SUCCESS != rltMsg.m_nResult)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_GETTASKEXTRAREWARD, &rltMsg);
		return;
	}

	MG_SyncTaskExtraReward syncMsg;
	syncMsg.m_nSyncType = SyncType_Delta;
	syncMsg.m_nSyncOP = SyncOP_Del;
	syncMsg.m_nRewardInfo.reserve(50);

	for(mem::map<int32_t, mem::map<int32_t, int32_t> >::iterator it=m_ExtraRewardData.begin(); it!=m_ExtraRewardData.end(); ++it)
	{
		mem::map<int32_t, int32_t>::iterator kIt = it->second.begin();
		while(kIt != it->second.end())
		{
			ResultType res  = LC_Helper::AddItem(pkPlayer->GetPackAsset(), kIt->first, kIt->second, 0, false, it->first);
			if(RE_SUCCESS == res)
			{
				KeyValue_Base data;
				data.key = kIt->first;
				data.value = kIt->second;
				syncMsg.m_nRewardInfo.push_back(data);

				kIt = it->second.erase(kIt);
			}
			else
			{
				rltMsg.m_nResult = RE_ITEM_PACKAGE_FULL;
				break;
			}
		}
	}

	if(!syncMsg.m_nRewardInfo.empty())
	{
		pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
		pkPlayer->SendMsgToClient(MGPT_SYNCTASKEXTRAREWARD, &syncMsg);
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_GETTASKEXTRAREWARD, &rltMsg);
}

void CTaskExtraReward::UpdateTaskExtraReward(int32_t itemID, int32_t itemNum, int32_t bindMode)
{
	mem::map<int32_t, mem::map<int32_t, int32_t> >::iterator findIt = m_ExtraRewardData.find(bindMode);
	if(findIt == m_ExtraRewardData.end())
	{
		mem::map<int32_t, int32_t> data;
		data[itemID] = itemNum;

		m_ExtraRewardData.insert(std::make_pair(bindMode, data));
	}
	else
	{
		mem::map<int32_t, int32_t>::iterator it = findIt->second.find(itemID);
		if(it == findIt->second.end())
		{
			findIt->second.insert(std::make_pair(itemID, itemNum));
		}
		else
		{
			it->second += itemNum;
		}
	}
}