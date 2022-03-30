#include "LC_ResourceRetrieveMgr.h"
#include "MG_SyncProtocol.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerPlayer.h"
#include "LC_TaskManager.h"
#include "LC_TaskMap.h"
#include "LC_Helper.h"
#include "CF_ComeBack.h"
#include "CF_EXPList.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;

namespace GameLogic
{
	LC_ResourceRetrieveMgr::LC_ResourceRetrieveMgr(): m_nActivateState(false), m_nOwnerPlayerId(0), m_nTimeStamp(0), m_nRewardPlayerLevel(0)
	{
		m_nRewardMapID.clear();
		m_nResourceRetrieveRecord.clear();
	}

	LC_ResourceRetrieveMgr::~LC_ResourceRetrieveMgr()
	{

	}

	void LC_ResourceRetrieveMgr::Save(char_data_define::char_ResourceRetrieve& info)
	{
		info.set_activestate(m_nActivateState);
		info.set_timestamp(m_nTimeStamp);

		for(mem::map<int, int>::iterator it=m_nResourceRetrieveRecord.begin(); it!=m_nResourceRetrieveRecord.end(); ++it)
		{
			::char_data_define::char_ResourceRetrieveRecord* data = info.add_resourceretrieverecord();
			data->set_id(it->first);
			data->set_times(it->second);
		}		

		for(mem::map<int, int>::iterator it=m_nRewardMapID.begin(); it!=m_nRewardMapID.end(); ++it)
		{
			::char_data_define::char_RewardMapID* data = info.add_rewardmapid();
			data->set_id(it->first);
			data->set_mapid(it->second);
		}

		info.set_rewardplayerlevel(m_nRewardPlayerLevel);
	}

	void LC_ResourceRetrieveMgr::Load(const char_data_define::char_ResourceRetrieve& info)
	{
		m_nActivateState = info.activestate();
		m_nTimeStamp = info.timestamp();

		m_nResourceRetrieveRecord.clear();
		for(int i=0; i<info.resourceretrieverecord_size(); ++i)
		{
			m_nResourceRetrieveRecord.insert(std::make_pair(info.resourceretrieverecord(i).id(), info.resourceretrieverecord(i).times()));
		}

		for(int i=0; i<info.rewardmapid_size(); ++i)
		{
			m_nRewardMapID.insert(std::make_pair(info.rewardmapid(i).id(), info.rewardmapid(i).mapid()));
		}

		m_nRewardPlayerLevel = info.rewardplayerlevel();
	}

	void LC_ResourceRetrieveMgr::Init(object_id_type nOwnerPlayerId)
	{
		m_nOwnerPlayerId = nOwnerPlayerId;
	}

	void LC_ResourceRetrieveMgr::ReUse()
	{
		m_nOwnerPlayerId = INVALID_ACTOR_ID;
		m_nActivateState = false;
		m_nTimeStamp = 0;
		m_nResourceRetrieveRecord.clear();
	}

	LC_ServerPlayer* LC_ResourceRetrieveMgr::GetOwnerPlayer()
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(m_nOwnerPlayerId);
		return pPlayer;
	}

	ResultType LC_ResourceRetrieveMgr::ActiveState()
	{
		if(true == m_nActivateState)
		{
			return RE_RESOURCE_RETRIEVE_1;
		}

		m_nActivateState = true;
		m_nTimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		return RE_SUCCESS;
	}

	bool LC_ResourceRetrieveMgr::IsActive()
	{
		return m_nActivateState;
	}

	ResultType LC_ResourceRetrieveMgr::GetResourceRetrieverReward(int id, bool specialFind)
	{
		//参数合法性
		CF_ComeBack::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ComeBack>(id);
		if(NULL == pkData)
		{
			return RE_RESOURCE_RETRIEVE_3;
		}
		
		LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
		if(NULL == pkOwnerPlayer)
		{
			return RE_RESOURCE_RETRIEVE_3;
		}

		//次数不足
		int times = GetVaildTimes(id);
		if(0 == times)
		{
			return RE_RESOURCE_RETRIEVE_2;
		}
		else if(times < 0)
		{
			return RE_RESOURCE_RETRIEVE_5;
		}

		ResultType res = RE_SUCCESS;
		if(specialFind)
		{
			//检测特权是否开启
			uint32_t res = LC_Helper::GetVipPrivilegeTimesByType(VIP_PC_RESOURCE_RETRIEVE, pkOwnerPlayer);
			if(res <= 0)
			{
				return RE_RESOURCE_RETRIEVE_4;
			}

			//钱够不
			res =  LC_Helper::CheckCashMap(pkOwnerPlayer, pkData->_iCost, times);
			if(RE_SUCCESS != res)
			{
				return res;
			}
		}
		
		int playerLevel = m_nRewardPlayerLevel;
		CF_EXPList::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_EXPList>(playerLevel);
		if(NULL == pkEntry)
		{
			return RE_RESOURCE_RETRIEVE_3;
		}

		int index = 0;
		UT_SIMDataList itemList;
		if(!pkData->_sLevel.empty())
		{
			switch(pkData->_iType)
			{
			case ResourceRetrieveType_Task:
			case ResourceRetrieveType_SoloDule:
			case ResourceRetrieveType_Activity:			
				{
					Utility::UT_SIMDataList::iterator slowIt=pkData->_sLevel.begin();
					Utility::UT_SIMDataList::iterator fastIt = pkData->_sLevel.begin();
					for(++fastIt; slowIt!=pkData->_sLevel.end() && fastIt!=pkData->_sLevel.end(); ++slowIt, ++fastIt, ++index)
					{
						if(playerLevel>= slowIt->IID() && playerLevel<fastIt->IID())
						{
							break;
						}
					}
				}
				break;
			case ResourceRetrieveType_GameStory_2:
			case ResourceRetrieveType_GameStory_5:
				{
					mem::map<int, int>::iterator findIt = m_nRewardMapID.find(pkData->_iID);
					if(findIt != m_nRewardMapID.end())
					{
						int rewardMapID = findIt->second;
						for(Utility::UT_SIMDataList::iterator it=pkData->_sLevel.begin(); it!=pkData->_sLevel.end(); ++it, ++index)
						{
							int mapID = it->IID();
							if(mapID == rewardMapID)
							{
								break;
							}
						}
					}
				}
				break;
			default:
				return RE_FAIL;
			}
		}

		GetResourceRetrieverRewardList(pkData->_kReward, index, itemList);

		float percent = specialFind ? pkData->_iSpecialNormal* 0.0001f : pkData->_iNormalPercent* 0.0001f;

		//道具奖励
		for(Utility::UT_SIMDataList::iterator it=itemList.begin(); it!=itemList.end(); ++it)
		{
			int oldTimes = it->ParamA();
			int newTimes = Round2Int(oldTimes*times*percent);
			it->SetParamA(newTimes);
		}

		UT_SIMDataInfo data;
		//指挥官经验		
		data.SetItemInfo(SPEC_ITEM_TID_EXP, Round2Int(pkEntry->_lBaseXP*pkData->_iExp*percent*times), 0);
		itemList.push_back(data);

		//buddy经验
		data.clear();
		data.SetItemInfo(SPEC_ITEM_TID_BUDDY_EXP, Round2Int(pkEntry->_lBaseBuddyXP*pkData->_iYuanneng*percent*times), 0);
		itemList.push_back(data);
		
		//金币
		data.clear();
		data.SetItemInfo(SPEC_ITEM_TID_CASH, Round2Int(pkEntry->_lBaseCash*pkData->_iGold*percent*times), 0);
		itemList.push_back(data);
		//资源找回--埋点日志
		vector<int> nLogParams;
		nLogParams.clear();
		nLogParams.push_back(id);
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, ZHAOHUIRESOURCE_OPERATE, OPERATE_LOGTYPE1);
		if(specialFind)
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, ZHAOHUIRESOURCE_OPERATE, OPERATE_LOGTYPE2);

		StringType mailTitle;
		StringType mailDesc;
		res = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, itemList, pkOwnerPlayer, SYS_FUNC_TP_RESOURCE_RETRIEVE,	pkData->_iID, mailTitle, mailDesc, false,false,false,nLogCode,nLogParams);
		if(RE_SUCCESS != res)
		{
			return res;
		}

		//帮会贡献
		pkOwnerPlayer->AddGuildMoney(Round2Int(pkData->_iGuild*percent*times), SYS_FUNC_TP_RESOURCE_RETRIEVE, 0);
		
		//勇武勋章
		pkOwnerPlayer->AddCouragePoint(Round2Int(pkData->_iCourage*percent*times), SYS_FUNC_TP_RESOURCE_RETRIEVE, 0);

		if(specialFind)
		{
			//扣钱
			res = LC_Helper::PayCashMap(pkOwnerPlayer, pkData->_iCost, SYS_FUNC_TP_RESOURCE_RETRIEVE, pkData->_iID, times,false,nLogCode);
			if(RE_SUCCESS != res)
			{
				return res;
			}
		}
		
		//设置已经找回过了
		SetVaildTimes(id, -1*times);
		return RE_SUCCESS;
	}

	void LC_ResourceRetrieveMgr::GetResourceRetrieverRewardList(StringType& reward, int index, Utility::UT_SIMDataList& itemList)
	{
		mem::vector<StringType> result;
		char str = '|';
		LC_Helper::SpliteString2MultiString(reward, result, str);
		if(index < result.size())
		{
			ParseFmtString2DataIFs(result[index], itemList);		
			return;
		}
	}

	int LC_ResourceRetrieveMgr::GetVaildTimes(int id)
	{
		mem::map<int, int>::iterator findIt = m_nResourceRetrieveRecord.find(id);
		return findIt!=m_nResourceRetrieveRecord.end() ? findIt->second : 0;
	}

	void LC_ResourceRetrieveMgr::SetVaildTimes(int id, int times)
	{
		mem::map<int, int>::iterator findIt = m_nResourceRetrieveRecord.find(id);
		if(findIt != m_nResourceRetrieveRecord.end())
		{
			findIt->second = times;
		}
	}

	void LC_ResourceRetrieveMgr::SyncResourceRetrieverRecord()
	{
		LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
		if(NULL == pkOwnerPlayer)
		{
			return;
		}

		MG_SyncResourceRetrieverRecord syncMsg;
		for(mem::map<int, int>::iterator it=m_nResourceRetrieveRecord.begin(); it!=m_nResourceRetrieveRecord.end(); ++it)
		{
			syncMsg.m_nResourceRetrieverRecord.insert(std::make_pair(it->first, it->second));
		}

		for(mem::map<int, int>::iterator it=m_nRewardMapID.begin(); it!=m_nRewardMapID.end(); ++it)
		{
			syncMsg.m_nRewardMapID.insert(std::make_pair(it->first, it->second));
		}

		syncMsg.m_nRewardPlayerLevel = m_nRewardPlayerLevel;

		pkOwnerPlayer->SendMsgToClient(MGPT_SYNCRESOURCERETRIEVERRECORD, &syncMsg);
	}

	void LC_ResourceRetrieveMgr::UpdateResourceRetrieveRecord()
	{
		LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
		if(NULL == pkOwnerPlayer)
		{
			return;
		}

		bool tag = IsActive();
		if(false == tag)
		{
			return;
		}

		uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		int32_t deltaDay  = LC_Helper::GetDeltaDay(m_nTimeStamp, currentTime);
		if(deltaDay > 0)
		{			
			m_nResourceRetrieveRecord.clear();
			m_nRewardMapID.clear();
			
			m_nRewardPlayerLevel = pkOwnerPlayer->GetLevel();
			LC_VIPCmnManager* pkMgr = LC_VIPCmnManager::GetSingletonPtr();
			if(pkMgr == NULL)
			{
				return;
			}

			CF_ComeBack::DataEntryMapExternIterator kIter = CF_ComeBack::GetSingletonPtr()->GetIterator();
			for(; kIter.HasMoreElements(); kIter.MoveNext())
			{
				CF_ComeBack::DataEntry* pkData = kIter.PeekNextValuePtr();
				bool tag = pkOwnerPlayer->GetOpenAbilityAsset().IsAbilityOpened(pkData->_iOpenID);
				if(false == tag)
				{
					continue;
				}

				int vaildTimes = 0;
				int usedTimes = 0;
				int vipExtraTimes = 0;
				int param1 = 0;				
				int param2 = 0;
				
				LC_VIPProfitDataEntry* pkDataEntry = pkMgr->GetVIPProfitDataEntry(pkData->_iVipProfitID);
				if(NULL != pkDataEntry)
				{
					vipExtraTimes = pkDataEntry->GetValue(pkOwnerPlayer->GetVIPLevel(), false);
				}

				switch(pkData->_iType)
				{
				case ResourceRetrieveType_Task:
					{
						if(deltaDay == 1)
						{
							bool open = false;						
							LC_TaskManager* pkTaskMgr = LC_TaskManager::GetSingletonPtr();
							LC_TaskMap* pkTaskMap = pkOwnerPlayer->GetTaskMap();
							if(NULL!=pkTaskMgr && NULL!=pkTaskMap)
							{
								Utility::UT_SIMDataList::iterator it = pkData->_iParam.begin();
								if(it == pkData->_iParam.end())
								{
									continue;
								}

								param1 = it->IID();
								uint32_t uiAssignType = pkTaskMgr->GetAssignType(param1);

								LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();
								//if (NULL!=pkAssignInfo)
								{
									LC_AssignTaskData*	assignData = pkAssignInfo->GetAssignTaskData(uiAssignType);
									if(NULL != assignData)
									{
										usedTimes = assignData->GetRound();
										//已经派发的公会任务在派发列表中(任务未接取/未完成/完成未领奖)，本次要参与资源找回
										int assignNum = pkAssignInfo->GetAssignTaskCount(uiAssignType);
										if(usedTimes>0 && assignNum != 0)
										{
											usedTimes -= 1;
										}
									}
								}
							}
						}						
						vaildTimes = pkData->_iTimes + vipExtraTimes - usedTimes;
					}
					break;
				case ResourceRetrieveType_GameStory_2:
					{
						Utility::UT_SIMDataList::iterator it = pkData->_iParam.begin();
						if(it == pkData->_iParam.end())
						{
							continue;
						}

						for(Utility::UT_SIMDataList::iterator kIt=pkData->_sLevel.begin(); kIt!=pkData->_sLevel.end(); ++kIt)
						{
							int mapID = kIt->IID();
							bool tag = pkOwnerPlayer->GetRaidAsset()->IsAlreadyPassRaid(mapID);
							if(false == tag)
							{
								m_nRewardMapID.insert(std::make_pair(pkData->_iID, mapID));
								break;
							}
						}

						param1 = it->IID();
						param2 = it->ParamA();
						if(1 == param2)
						{//通关扣次数
							usedTimes = (deltaDay==1) ? (pkOwnerPlayer->GetGameStoryFinishTimes(param1)) : 0;
						}
						else if(0 == param2)
						{//进入扣次数
							usedTimes = (deltaDay==1) ? (pkOwnerPlayer->GetGameStoryEnterTimes(param1)) : 0;
						}
						
						vaildTimes = pkData->_iTimes + vipExtraTimes - usedTimes;
					}
					break;
				case ResourceRetrieveType_GameStory_5:
					{
						Utility::UT_SIMDataList::iterator it = pkData->_iParam.begin();
						if(it == pkData->_iParam.end())
						{
							continue;
						}

						int maxMapID = 0;
						for(Utility::UT_SIMDataList::iterator kIt=pkData->_sLevel.begin(); kIt!=pkData->_sLevel.end(); ++kIt)
						{							
							int mapID = kIt->IID();
							if(maxMapID == 0)
							{
								maxMapID = mapID;
							}

							bool tag = pkOwnerPlayer->GetRaidAsset()->IsAlreadyPassRaid(mapID);
							if(true==tag && mapID>maxMapID)
							{
								maxMapID = mapID;
							}
						}

						m_nRewardMapID.insert(std::make_pair(pkData->_iID, maxMapID));
						param1 = it->IID();
						param2 = it->ParamA();
						if(1 == param2)
						{//通关扣次数
							usedTimes = (deltaDay==1) ? (pkOwnerPlayer->GetGameStoryFinishTimes(param1)) : 0;
						}
						else if(0 == param2)
						{//进入扣次数
							usedTimes = (deltaDay==1) ? (pkOwnerPlayer->GetGameStoryEnterTimes(param1)) : 0;
						}

						vaildTimes = pkData->_iTimes + vipExtraTimes - usedTimes;
					}
					break;
				case ResourceRetrieveType_SoloDule:
					{
						usedTimes = (deltaDay==1) ? (pkOwnerPlayer->GetArenaBattleAssert()->GetLocalChallengedTime()) : 0;
						vaildTimes = pkData->_iTimes + vipExtraTimes - usedTimes;
					}
					break;
				case ResourceRetrieveType_Activity:
					{
						Utility::UT_SIMDataList::iterator it = pkData->_iParam.begin();
						if(it == pkData->_iParam.end())
						{
							continue;
						}

						param1 = it->IID();
						usedTimes = (deltaDay==1) ? (pkOwnerPlayer->GetGameStoryJoinTimes(param1)) : 0;
						vaildTimes = pkData->_iTimes + vipExtraTimes - usedTimes;
					}
					break;
				default:
					break;
				}

				vaildTimes = vaildTimes > 0 ? vaildTimes : 0;
				m_nResourceRetrieveRecord.insert(std::make_pair(pkData->_iID, vaildTimes));
			}
			m_nTimeStamp = currentTime;
			SyncResourceRetrieverRecord();
		}
	}

	uint32_t LC_ResourceRetrieveMgr::GetTimeStamp()
	{
		return m_nTimeStamp;
	}

	void LC_ResourceRetrieveMgr::SetTimeStamp(uint32_t time)
	{
		m_nTimeStamp = time;
	}

	void LC_ResourceRetrieveMgr::SyncState()
	{
		LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
		if (NULL == pkOwnerPlayer)
		{
			return;
		}

		MG_SyncResourceRetrieveState msg;
		msg.m_bActiveState = m_nActivateState;
		pkOwnerPlayer->SendMsgToClient(MGPT_SYNCRESOURCERETRIEVESTATE, &msg);
	}
}