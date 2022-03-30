#include "LC_QuickRaid.h"
#include "LC_ServerPlayer.h"
#include "LC_GameEventManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_GameStory_Manager.h"
#include "CF_WorldMapList.h"

CQuickRaidAsset::CQuickRaidAsset()
{
	Clear();
}

CQuickRaidAsset::~CQuickRaidAsset()
{
	Clear();
}

void CQuickRaidAsset::Clear()
{
	m_nUsedRaidTimesRecords.clear();
}

void CQuickRaidAsset::SaveData(::char_data_define::PBQuickRaidData* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	for(mem::map<int32_t, uint32_t>::iterator it=m_nUsedRaidTimesRecords.begin(); it!=m_nUsedRaidTimesRecords.end(); ++it)
	{
		::char_data_define::char_UsedRaidTimesRecords* data = pkData->add_usedraidtimesrecords();
		if(NULL != data)
		{
			data->set_mapid(it->first);
			data->set_useraidtimes(it->second);
		}		
	}
}

void CQuickRaidAsset::LoadData(const ::char_data_define::PBQuickRaidData& data)
{
	for(int i=0; i<data.usedraidtimesrecords_size(); ++i)
	{
		m_nUsedRaidTimesRecords[data.usedraidtimesrecords(i).mapid()] = data.usedraidtimesrecords(i).useraidtimes();
	}
}

void CQuickRaidAsset::QuickRaid_New(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID,  uint32_t times)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RLT_QuickRaid_New resp;
	resp.m_nResMapID = mapID;
	resp.m_nRaidNum = times;
	resp.m_nResult = RE_SUCCESS;
	
	do 
	{
		if(times < 1)
		{
			resp.m_nResult = RE_QUICK_RAID_ERROR_1;
			break;
		}

		CF_TranscriptionRule::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(mapID);
		CF_WorldMapList::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapID);
		if(NULL==pkData || NULL==pkEntry)
		{
			resp.m_nResult = RE_QUICK_RAID_ERROR_1;
			break;
		}

		resp.m_nResult = CheckQuick_Raid(pkOwnerPlayer, mapID, times, pkEntry->_iStoryID, pkData);
		if(RE_SUCCESS != resp.m_nResult)
		{
			break;
		}
		
		resp.m_nResult = DoQuick_Raid_New(pkOwnerPlayer, mapID, times, pkEntry->_iStoryID, pkData);
		if(RE_SUCCESS != resp.m_nResult )
		{
			break;
		}

		AddQuickRaidTimes(pkOwnerPlayer, mapID, times,  pkEntry->_iStoryID);

		//event 
		{
			LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
			LC_GameEvent* pkGameEvent1 = pkEventManager->AllocGameEvent();
			if (pkGameEvent1 != NULL)
			{
				pkGameEvent1->SetEventType(GLET_TRANSCRIPTION_EVENT);
				pkGameEvent1->SetEventParam(EP_TRANSCRIPTION_EVENT_TYPE, EVENT_TYPE_FINISH);
				pkGameEvent1->SetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_1, pkOwnerPlayer->GetID());
				pkGameEvent1->SetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_2, mapID);
				pkEventManager->InsertGameEvent(pkGameEvent1);
			}

			LC_GameEvent* pkGameEvent2 = pkEventManager->AllocGameEvent();
			if (pkGameEvent2 != NULL)
			{
				pkGameEvent2->SetEventType(GLET_RAIDMAP);
				pkGameEvent2->SetEventParam(EP_RAIDMAP_PLAYER_ID, pkOwnerPlayer->GetID());
				pkGameEvent2->SetEventParam(EP_RAIDMAP_MAPID, mapID);
				pkGameEvent2->SetEventParam(EP_RAIDMAP_TIMES, times);
				pkEventManager->InsertGameEvent(pkGameEvent2);
			}
		}
	} while (false);
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_QUICKRAID_NEW, &resp);
}

ResultType CQuickRaidAsset::CheckQuick_Raid(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times, int32_t storyID, CF_TranscriptionRule::DataEntry* pkData)
{
	bool tag = UT_ServerHelper::CheckStoryIsActive(storyID);
	if(!tag)
	{
		return RE_DOMAIN_NOT_OPEN;
	}

	const RaidInfo* pkRaid = pkOwnerPlayer->GetRaidAsset()->FindRaid(mapID);
	if(pkRaid == NULL)
	{
		return RE_QUICK_RAID_NOT_EXIST;
	}

	ResultType res = RE_SUCCESS;
	//检测挑战资格
	switch(storyID)
	{
	case GameStoryIDType_GloryExpedition:
		{
			//荣光远征三星通关才能扫荡
			if(!pkData->_Stage.empty() && pkRaid->Score() != 3)
			{
				res = RE_QUICK_RAID_BE_LIMIETD;
			}

			//荣光远征的扫荡次数配表pkData->_Times_Quick
			uint32_t usedTime = GetUsedRaidTimes(mapID);
			if(usedTime+times > pkData->_Times_Quick)
			{
				res = RE_QUICK_RAID_TIMES_BE_LIMITED;
			}
		}
		break;
	case GameStoryIDType_Sub1:
	case GameStoryIDType_Sub2:
	case GameStoryIDType_Sub3:
		{
			//进阶副本，只能扫荡目前挑战的最高的地图id
			int32_t maxFinishMapID = pkOwnerPlayer->GetFinishMaxMapIDByStoryID(storyID);
			if(0==maxFinishMapID || maxFinishMapID != mapID)
			{
				return RE_QUICK_RAID_ERROR_1;
			}

			//进阶副本，扫荡和挑战共用次数
			GameStory::LC_GameStory_Manager* pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
			res = pkGSMgr->CheckGameStoryEnterCondition(pkOwnerPlayer, storyID, times);
		}
	}

	if(RE_SUCCESS != res)
	{
		return res;
	}

	//检测扫荡道具是否足够
	Utility::UT_SIMDataList consumeItem = pkData->_Consume_Quick;
	if(!consumeItem.empty())
	{
		for(Utility::UT_SIMDataList::iterator it=consumeItem.begin(); it!=consumeItem.end(); ++it)
		{
			int num = it->ItemCount();
			it->ItemCount(num*times);
		}

		res = LC_Helper::IsItemsEnough(pkOwnerPlayer->GetPackAsset(), consumeItem);
		if(RE_SUCCESS != res)
		{
			return res;
		}
	}

	//检测扫荡货币是否足够
	if(!pkData->_Consume_Quick_Money.empty())
	{
		for(Utility::UT_SIMDataList::iterator it=pkData->_Consume_Quick_Money.begin(); it!=pkData->_Consume_Quick_Money.end(); ++it)
		{
			bool tag = pkOwnerPlayer->IsCashEnough(it->ParamA()*times, it->IID());
			if(!tag)
			{
				return RE_FAIL;
			}
		}
	}

	return RE_SUCCESS;
}

ResultType CQuickRaidAsset::DoQuick_Raid_New(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times, int32_t storyID, CF_TranscriptionRule::DataEntry* pkData)
{
	mem::map<int32_t, int32_t> bindItemInfo;
	mem::map<int32_t, int32_t> unbindItemInfo;
	for(int i=0; i<times; ++i)
	{
		for(Utility::UT_SIMDataList::iterator it=pkData->_Reward_Quick.begin(); it!=pkData->_Reward_Quick.end(); ++it)
		{
			int percent = it->ParamC();
			int random = (percent==10000) ? percent : rand()%10000;
			if(random <= percent)
			{
				int32_t itemID = it->IID();
				int32_t itemNum = it->ParamA();
				int32_t bindType = it->ParamB();
				if(bindType)
				{
					mem::map<int32_t, int32_t>::iterator findIt = bindItemInfo.find(itemID);
					if(findIt == bindItemInfo.end())
					{
						bindItemInfo[itemID] = itemNum;
					}
					else
					{
						findIt->second += itemNum;
					}
				}
				else
				{
					mem::map<int32_t, int32_t>::iterator findIt = unbindItemInfo.find(itemID);
					if(findIt == unbindItemInfo.end())
					{
						unbindItemInfo[itemID] = itemNum;
					}
					else
					{
						findIt->second += itemNum;
					}
				}				
			}
		}		
	}

	//扫荡奖励
	Utility::UT_SIMDataList rewardItem;
	for(mem::map<int32_t, int32_t>::iterator it=bindItemInfo.begin(); it!=bindItemInfo.end(); ++it)
	{
		UT_SIMDataInfo itemData;
		itemData.SetItemInfo(it->first, it->second, 1);
		rewardItem.push_back(itemData);
	}
	for(mem::map<int32_t, int32_t>::iterator it=unbindItemInfo.begin(); it!=unbindItemInfo.end(); ++it)
	{
		UT_SIMDataInfo itemData;
		itemData.SetItemInfo(it->first, it->second, 0);
		rewardItem.push_back(itemData);
	}

	int playerlevel = pkOwnerPlayer->GetLevel();
	int cashNum = LC_Helper::BaseLevelCash(playerlevel)*pkData->_Money_Quick*times;
	int controllerExpNum = LC_Helper::BaseLevelExp(playerlevel)*pkData->_ControllerExp_Quick*times;
	int buddyExpNum = LC_Helper::BaseLevelBuddyExp(playerlevel)*pkData->_BuddyExp_Quick*times;

	//金币
	if(cashNum > 0)
	{
		Utility::UT_SIMDataInfo kCashItem;
		kCashItem.SetItemInfo(SPEC_ITEM_TID_CASH, cashNum);
		rewardItem.push_back(kCashItem);
	}	

	//指挥官经验
	if(controllerExpNum > 0)
	{
		Utility::UT_SIMDataInfo kControllerExpItem;
		kControllerExpItem.SetItemInfo(SPEC_ITEM_TID_EXP, controllerExpNum);
		rewardItem.push_back(kControllerExpItem);
	}

	//buddy经验
	if(buddyExpNum > 0)
	{
		Utility::UT_SIMDataInfo kBuddyExpItem;
		kBuddyExpItem.SetItemInfo(SPEC_ITEM_TID_BUDDY_EXP, buddyExpNum);
		rewardItem.push_back(kBuddyExpItem);
	}

	vector<int> nParams;
	nParams.clear();
	nParams.push_back(mapID);
	nParams.push_back(times);
	//扫荡--埋点日志
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN9_OPERATE, OPERATE_LOGTYPE2);

	//发奖
	ResultType res = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, rewardItem, pkOwnerPlayer, SYS_FUNC_TP_RAID_QUICK, mapID, pkData->_Quick_mail_Title, pkData->_Quick_mail_Desc, pkData->_Quick_mail, false, false, nLogCode, nParams);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	//删除道具
	for(Utility::UT_SIMDataList::const_iterator it=pkData->_Consume_Quick.begin(); it!=pkData->_Consume_Quick.end(); ++it)
	{
		pkOwnerPlayer->DeleteItem(it->IID(), it->ItemCount()*times, nLogCode, nParams);
	}

	//扣货币
	for(Utility::UT_SIMDataList::iterator it=pkData->_Consume_Quick_Money.begin(); it!=pkData->_Consume_Quick_Money.end(); ++it)
	{
		pkOwnerPlayer->ReduceCash(it->ParamA()*times, (CashType)(it->IID()), SYS_FUNC_TP_QUICK_RAID, mapID, nLogCode, nParams);
	}

	int32_t rewardType = 0;
	do 
	{
		switch(storyID)
		{
		case GameStoryIDType_GloryExpedition:
			{
				rewardType = RewardType_QucikRaidRongGuangYuanZheng;
			}
			break;
		default:
			break;
		}
	} while (false);

	//通知客户端奖励
	if(rewardType > 0)
	{
		StringType reward;
		FormatDataIFs2String(rewardItem, reward);
		MG_SyncReward msg;
		msg.m_nResult = RE_SUCCESS;
		msg.m_nType = rewardType;
		msg.m_nReward = TPSTR2STDSTR(reward);
		pkOwnerPlayer->SendMsgToClient(MGPT_SYNCREWARD, &msg);
	}
	return RE_SUCCESS;
}

void CQuickRaidAsset::AddQuickRaidTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times, int32_t storyID)
{
	switch(storyID)
	{
	case GameStoryIDType_GloryExpedition:
		{
			//扫荡和挑战，分开计数
			AddUsedRaidTimes(pkOwnerPlayer, mapID, times);
		}
		break;
	case GameStoryIDType_Sub1:
	case GameStoryIDType_Sub2:
	case GameStoryIDType_Sub3:
		{
			//扫荡和挑战，共用次数
			GameStory::LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
			GameStory::LC_Server_GameStory_Base* pkStory = pkGSMgr->GetGameStoryBasePtrByStoryID(storyID);
			if(NULL != pkStory)
			{
				pkStory->PlayerFinish(pkOwnerPlayer, mapID, times);
			}
		}
		break;
	default:
		break;
	}
}

uint32_t CQuickRaidAsset::GetUsedRaidTimes(int32_t key)
{
	mem::map<int32_t, uint32_t>::iterator findIt=m_nUsedRaidTimesRecords.find(key);
	if(findIt == m_nUsedRaidTimesRecords.end())
	{
		return 0;
	}

	return findIt->second;
}

void CQuickRaidAsset::AddUsedRaidTimes(LC_ServerPlayer* pkOwnerPlayer, int32_t key, uint32_t times)
{
	uint32_t num = 0;
	mem::map<int32_t, uint32_t>::iterator findIt=m_nUsedRaidTimesRecords.find(key);
	if(findIt == m_nUsedRaidTimesRecords.end())
	{
		m_nUsedRaidTimesRecords[key] = times;
		num = times;
	}
	else
	{
		findIt->second += times;
		num = findIt->second;
	}

	SyncUsedRaidCount(pkOwnerPlayer, key, num);
}

void CQuickRaidAsset::ResetQuickRaidTimes(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	for(mem::map<int32_t, uint32_t>::iterator it=m_nUsedRaidTimesRecords.begin(); it!=m_nUsedRaidTimesRecords.end(); ++it)
	{
		it->second = 0;
	}

	SyncQuickRaidAsset(pkOwnerPlayer);
}

void CQuickRaidAsset::SyncQuickRaidAsset(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_SyncUsedRaidCount msg;
	for(mem::map<int32_t, uint32_t>::iterator it=m_nUsedRaidTimesRecords.begin(); it!=m_nUsedRaidTimesRecords.end(); ++it)
	{
		msg.m_nUsedRaidTimesRecords[it->first] = it->second;
	}

	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCUSEDRAIDCOUNT, &msg);
}


void CQuickRaidAsset::SyncUsedRaidCount(LC_ServerPlayer* pkOwnerPlayer, int32_t key, uint32_t times)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_SyncUsedRaidCount msg;
	msg.m_nUsedRaidTimesRecords[key] = times;
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNCUSEDRAIDCOUNT, &msg);
}