#include "LM_LocaleManager.h"
#include "GameLogicScriptPCH.h"
#include "LC_Task.h"
#include "LC_ServerTask.h"
#include "LC_ServerItemFlopManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_GameEvent.h"
#include "LC_ServerNPC.h"
#include "LC_ServerGuildManager.h"
#include "UT_ServerHelper.h"
#include "GameServerApp.h"
#include "LC_PackEntryFactory.h"
#include "CF_CharType.h"
#include "MG_Common.h"
#include "LC_LogicTriggerManager.h"
#include "TS_TranManager.h"
#include "LC_ItemFactoryBase.h"
#include "LC_ServerItemFlopManager.h"
#include "LC_ServerWorldManager.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace GameLogic;
using namespace CSVFile;
using namespace Utility;
//--------------------------------------------------------
LC_ServerTask::LC_ServerTask()
	: LC_Task()
{
}
//--------------------------------------------------------
LC_ServerTask::~LC_ServerTask()
{
}
//--------------------------------------------------------
void LC_ServerTask::ResolveParams(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry)
{
	if (NULL == pkPlayer || NULL == pkTaskEntry)
	{
		return;
	}
	LC_TaskParams& taskParams = pkTaskEntry->GetTaskParam();
	if (m_kParam.IsValid())
	{
		//先完全随机吧
		if (!m_kParam._kDiffiStarCons.empty())
		{
			taskParams.SetDiffiStar(UT_MathBase::RandInRangeInt(0, (int)m_kParam._kDiffiStarCons.size() - 1));
		}
		taskParams.SetRewardStar(UT_MathBase::RandInRangeInt(1, m_kParam._iMaxRewardStar));
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}
//--------------------------------------------------------
void LC_ServerTask::RecordTaskOSS(int iType, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry)
{
}

//--------------------------------------------------------
void LC_ServerTask::NotifyEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkGameEvent == NULL || pkTaskEntry == NULL)
	{
		return;
	}
	if (m_kActiveTime > 0 && GET_PROCESS_TIME - pkTaskEntry->GetTaskStartTime() > m_kActiveTime)
	{
		return;
	}
	int iEventType = pkGameEvent->GetEventType();
	//ResultType iResult = -2;
	switch (iEventType)
	{
		case (GLET_KILL):
			{
				_ProcessKillNPCEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_GATHER_TASKNPC):
			{
				_ProcessGatherTaskNPCEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_LOGIN_IN):
			{
				_ProcessLoginInEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_CHALLENGE_WIN):
			{
				_ProcessChallengeWinEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_VIPLEVELUP):
			{
				_ProcessVIPUpLevel(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_VIPEXTARE_REWARD):
			{
				_ProcessVipExtareReward(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_TREASURE_TIMES):
			{
				_ProcessTreasureTimes(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_ADD_GUILD):
			{
				_ProcessAddGuild(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_TITLE_LEVEL):
			{
				_ProcessTitleLevel(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_KILLBOSS_DROPTIMES):
			{
				_ProcessKillBossDropTimes(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_ALIVE_BUDDYNUM):
			{
				_ProcessAliveBuddyNum(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_RAIDMAP):
			{
				_ProcessRaidMapTimes(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_PRESENT_BUDDYELEM):
			{
				_ProcessPresentBuddyElem(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_PLAYER_LEVEL_UP):
			{
				_ProcessControllerLevel(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_KILL_CONTROLLER):
			{
				_ProcessKillControllerEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_KILL_GROUP_NPC):
			{
				_ProcessKillGroupNPCEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_TALK_TO_NPC):
			{
				_ProcessTalkToNPCEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_PLAYER_ARRIVE_AREA):
			{
				_ProcessTaskAreaEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_PLAYER_PICK_ITEM):
			{
				_ProcessPickItemEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_TASK_TIME_TICK):
			{
				_ProcessTimeClickEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_USE_ITEM):
			{
				_ProcessUseItemEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_PLAYER_DIE):
			{
				_ProcessPlayerDieEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_SKILL_EXECUTE):
			{
				_ProcessSkillExecuteEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_TRANSCRIPTION_EVENT):
			{
				_ProcessTranscriptionEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_FINISH_TASK):
			{
				_ProcessFinishTaskEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_GAME_STORY_PROGRESS):
			{
				_ProcessStoryProcessEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_CROSS_ARENA_CHALLENGE):
		case (GLET_ARENA_CHALLENGE):
			{
				_ProcessChallengeEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_ADD_CASH):
			{
				_ProcessAddCashEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_TOTAL_ADD_CASH):
			{
				_ProcessTotalAddCashEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_REDUCE_CASH):
			{
				_ProcessReduceCashEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_SP_ADD):
			{
				_ProcessSPEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_COMBO_COUNT):
			{
				_ProcessComboEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_ITEM_IDENTIFY):
			{
				_ProcessItemIdentifyEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_KILL_ELITE):
			{
				_ProcessKillEliteEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_ADD_ITEM):
			{
				_ProcessItemGotEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_SWITCH_ITEM):
			{
				_ProcessItemSwitchEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_COMMON_STAT_OP):
			{
				_ProcessCommonStatOPEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_TREASURE_CHEST_OP):
			{
				_ProcessTreasureChestOPEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_SIGNIN):
			{
				_ProcessSigninEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_GUILD_OFFER):
			{
				_ProcessGuildOfferEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_KILL_PROGRESS):
			{
				_ProcessGuildKillEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_ITEM_OPERATION):
			{
				_ProcessItemOperationEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		case (GLET_ACTI_STAT_OP):
			{
				_ProcessActiStatOPEvent(pkGameEvent, pkPlayer, pkTaskEntry, kFilter);
			}
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------------
void LC_ServerTask::GenerateTaskItemFlopInfoOnNPCDead(LC_ServerNPC* pkNPC, GameLogic::LC_PlayerBase* pkPlayer
		, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry
		, LC_ItemFlopInfoList& rkFlopInfoList)
{
	//为了解决普通物品与任务物品掉落问题，并且减少逻辑事件前置带来的影响，将任务物品掉落从怪物死亡事件中分离
	if (pkPlayer == NULL || pkNPC == NULL || pkTaskEntry == NULL)
	{
		return;
	}
	if (m_kActiveTime > 0 && GET_PROCESS_TIME - pkTaskEntry->GetTaskStartTime() > m_kActiveTime)
	{
		return;
	}
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (!pkPackEntryFactory)
	{
		return;
	}
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	int lNPCTypeID = pkNPC->GetCharType();//pkGameEvent->GetEventParam(EP_KILL_NPC_CHAR_TYPE_ID);
	object_id_type lNPCID = pkNPC->GetID();//pkGameEvent->GetEventParam(EP_KILL_NPC_ID);
	bool bDrop = false;
	for (int i = 0; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition)
		{
			continue;
		}

		if (pkCondition->GetToFlop())
		{
			//TaskCondition.xlsx中字段_ToFlop修改为填入掉落ID：ItemGenMonsterDrop.xlsx中的_iID
			//pkFlopMgr->GenerateExtraRewaredOnNPCGlobal((LC_ServerNPC*)pkNPC);

			LC_ServerItemFlopManager* pkFlopMgr = LC_ServerItemFlopManager::GetSingletonPtr();
			pkFlopMgr->GenerateRewaredOnNPCDeadExtraDrop(pkNPC, pkCondition->GetToFlop());
		}

		if (TFT_ICOUNT != pkCondition->GetType())
		{
			continue;
		}

		if (lNPCTypeID != pkCondition->GetDropCharTypeID())
		{
			continue;
		}

		int iRandNum = UT_MathBase::RandInRangeInt(0, 100);
		if (iRandNum > pkCondition->GetDropItemChance())
		{
			continue;
		}

		if (pkNetPlayer->GetMapLogicID() == pkNPC->GetMapLogicID())
		{
			int lLength = UT_MathBase::LineLengthXYInt(pkNPC->GetCurrentLocation(), pkNetPlayer->GetCurrentLocation());
			if (lLength >= 0 && lLength < MAX_TASK_EVENT_SHARE_DISTANCE)
			{
				LC_ItemFlopInfo* pkFlopInfo = NULL;
				pkFlopInfo = rkFlopInfoList.FindItemFlopInfoByOwnerID(pkNetPlayer->GetID(), 1);
				if (!pkFlopInfo)
				{
					pkFlopInfo = rkFlopInfoList.CreateItemFlop(lNPCID, lNPCTypeID, 1, pkNPC->GetMapLogicID(), pkNPC->GetCurrentLocation());
				}
				pkFlopInfo->AddOwnerID(pkPlayer->GetID());

				pkFlopInfo->SetLifeTime(300.0f);

				int lTaskItemID			  = pkCondition->GetDropItemTypeID();
				int lTaskItemDropCount = pkCondition->GetDropItemNumber();
				int lTaskItemNeedCount = pkCondition->GetDropItemCount();

				if (lTaskItemDropCount <= 0 || !IS_ITEM_ID(lTaskItemDropCount))
				{
					continue;
				}

				int lHasTaskItemCount = pkPlayer->GetPackAsset()->GetCountCanDeleteInPack(lTaskItemID);
				int lRemainTaskItemCount = lTaskItemNeedCount - lHasTaskItemCount;
				if (0 >= lRemainTaskItemCount)
				{
					continue;
				}
				if (lRemainTaskItemCount < lTaskItemDropCount)
				{
					lTaskItemDropCount = lRemainTaskItemCount;
				}
				LC_BackPackEntryList kEntryList;
				pkPackEntryFactory->CreateItemEntry(pkCondition->GetDropItemTypeID(), pkCondition->GetDropItemNumber(), CIET_NONE, kEntryList, -1, 0, 0, true);
				PackOperateInfoList objOPList;
				for (int j = 0 ; j < (int)kEntryList.size() ; ++j)
				{
					PackOperateInfo objData;
					objData.Reset();

					objData.m_iOPType = OPT_ADD_ITEM;
					objData.m_bOPEntry = true;
					objData.m_bMerger  = true;
					objData.m_kPackEntry.Clone(kEntryList[j]);
					objOPList.push_back(objData);
				}

				ResultType iResult = pkPlayer->GetPackAsset()->ExecutePackOP(objOPList, true);
				if (RE_SUCCESS == iResult)
				{
					pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
					iResult = RE_PRODUCTION_SUCCESS;
					PackOPRecordSet* record_set = pkPlayer->GetPackAsset()->GetRecordSet();
					pkNetPlayer->HandlePackOPRecord(record_set, SYS_FUNC_TP_TASK_FLOP, GetTaskID());
				}
				else
				{
					Protocol::MG_Message errorMsg;
					errorMsg.m_iMessageID = iResult;
					errorMsg.m_iParameter1 = pkCondition->GetDropItemTypeID();
					errorMsg.m_iParameter2 = pkCondition->GetDropItemNumber();

					//LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
					pkNetPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
				}

			}
		}
		bDrop = true;
	}
}
//------------------------------------------------------------------------
void LC_ServerTask::_taskFininshGuildOperation(GameLogic::LC_PlayerBase* pkPlayer)
{}
ResultType LC_ServerTask::ReduceDiffStar(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry)
{
	if (NULL == pkPlayer || NULL == pkTaskEntry)
	{
		return RE_FAIL;
	}
	LC_TaskParams& taskParams = pkTaskEntry->GetTaskParam();
	ResultType rslt = RE_SUCCESS;
	do
	{
		if (!m_kParam.IsValid())
		{
			rslt = RE_TASK_PARAM_NOT_VALID;
			break;
		}
		if (taskParams.GetDiffiStar() <= 0)
		{
			rslt = RE_TASK_MIN_DIFF_STAR;
			break;
		}
		int iCash = GetReduceDiffCash(pkPlayer);
		if (iCash <= 0)
		{
			rslt = RE_UNVALID_CASH;
			break;
		}
		if (!pkPlayer->IsCashEnough(iCash, CT_UNBIND_CASH))
		{
			rslt = RE_TASK_CASH_NOT_ENOUGH;
			break;
		}
		pkPlayer->ReduceCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_TASK_DIFF, GetTaskID());
		taskParams.SetDiffiStar(0);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);

	} while (0);
	return RE_SUCCESS;
}
ResultType LC_ServerTask::PromoteRewardStar(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry)
{
	if (NULL == pkPlayer || NULL == pkTaskEntry)
	{
		return RE_FAIL;
	}
	LC_TaskParams& taskParams = pkTaskEntry->GetTaskParam();
	ResultType rslt = RE_SUCCESS;
	do
	{
		if (!m_kParam.IsValid())
		{
			rslt = RE_TASK_PARAM_NOT_VALID;
			break;
		}
		if (taskParams.GetRewardStar() >= m_kParam._iMaxRewardStar)
		{
			rslt = RE_TASK_MAX_REWARD_STAR;
			break;
		}
		int iCash = m_kParam.GetPromoteRewardCash(pkPlayer);
		if (iCash <= 0)
		{
			rslt = RE_UNVALID_CASH;
			break;
		}
		if (!pkPlayer->IsCashEnough(iCash, CT_UNBIND_CASH))
		{
			rslt = RE_TASK_CASH_NOT_ENOUGH;
			break;
		}
		pkPlayer->ReduceCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_TASK_REWARD, GetTaskID());
		taskParams.SetRewardStar(m_kParam._iMaxRewardStar);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);

	} while (0);
	return RE_SUCCESS;
}

class OneKeyCBArg : public TS_PostCommit
{
public:
	uint32_t m_uiCharID;
	LC_TaskParams& m_kTaskParams;
	int	m_iStar;
	int m_iTaskID;

	OneKeyCBArg(uint32_t uiCharID, LC_TaskParams& kTaskParams, int iStar, int iTaskID)
		: m_uiCharID(uiCharID)
		, m_kTaskParams(kTaskParams)
		, m_iStar(iStar)
		, m_iTaskID(iTaskID)
	{}

	virtual void operator()(void)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(m_uiCharID);
		if (NULL != pkPlayer)
		{
			pkPlayer->FinishTaskOP(m_iTaskID, 10000, true);
		}
	}
};

ResultType LC_ServerTask::OneKeyOperationQuickFinish(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry)
{
	if (NULL == pkPlayer || NULL == pkTaskEntry)
	{
		return RE_FAIL;
	}
	//LC_TaskParams& taskParams = pkTaskEntry->GetTaskParam();
	ResultType rslt = RE_SUCCESS;
	do
	{
		if (!m_kParam.IsValid())
		{
			rslt = RE_TASK_PARAM_NOT_VALID;
			break;
		}
		if (m_kParam._iOneKeyShopID <= 0)
		{
			rslt = RE_TASK_NOT_VALID_SHOPID;
			break;
		}
		LC_ShopIDMap kShopIDMap;
		kShopIDMap[m_kParam._iOneKeyShopID] = 1;
		rslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
		if (rslt != RE_SUCCESS)
		{
			break;
		}
		LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_TASK_ONEKEY, GetTaskID());
		pkPlayer->FinishTaskOP(GetTaskID(), 10000, true);
	} while (0);
	return rslt;
}
ResultType LC_ServerTask::OneKeyOperationMultiFinish(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry)
{
	if (NULL == pkPlayer || NULL == pkTaskEntry)
	{
		return RE_FAIL;
	}

	bool flag = true;
	//判断该任务是不是日常任务或者公会任务
	//判断该玩家的VIP等级是否可以领两倍奖励
	int taskTitleType = GetTaskTitleType();
	switch(taskTitleType)
	{
	case DAILY_TASK:
		flag = pkPlayer->GetVIPAsset().GetProfitValue(VIP_PROFIT_DAILYTASK_DOUBLE_REWARD)>0 ?  true: false;
		break;
	case GUILD_TASK:
		flag = pkPlayer->GetVIPAsset().GetProfitValue(VIP_PROFIT_GUILDTASK_DOUBLE_REWARD)>0 ?  true: false;
		break;
	default:
		break;
	}
	vector<int> nParams;
	nParams.clear();
	int nTaskId = GetTaskID();
	nParams.push_back(nTaskId);
	int nLogCode = 0;
	{
		switch(taskTitleType)
		{
		case DAILY_TASK:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, DAILYTASK_OPERATE, OPERATE_LOGTYPE2);
			break;
		case MAIN_TASK:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MAINTASK_OPERATE, OPERATE_LOGTYPE2);
			break;
		case GUILD_TASK:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, GUILDTASK_OPERATE, OPERATE_LOGTYPE2);
			break;
		default:
			break;
		}
	}
	
	if(!flag)
	{
		return RE_VIP_LEVEL_LACK;
	}

	//LC_TaskParams& taskParams = pkTaskEntry->GetTaskParam();
	ResultType rslt = RE_SUCCESS;
	do
	{
		if (!m_kParam.IsValid())
		{
			rslt = RE_TASK_PARAM_NOT_VALID;
			break;
		}
		if (m_kParam._iMultiFinishShopID <= 0)
		{
			rslt = RE_TASK_NOT_VALID_SHOPID;
			break;
		}
		rslt = CanFinish(pkPlayer, GET_PROCESS_TIME);
		if (RE_TASK_FINISH_SUCCESS != rslt)
		{
			break;
		}
		LC_ShopIDMap kShopIDMap;
		kShopIDMap[m_kParam._iMultiFinishShopID] = 1;
		rslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
		if (rslt != RE_SUCCESS)
		{
			break;
		}
		LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_TASK_FINISH_M, GetTaskID(),NULL,nLogCode, nParams);
		pkPlayer->FinishTaskOP(GetTaskID(), 20000, true);
	} while (0);
	return rslt;
}
ResultType LC_ServerTask::OneKeyOperationComboFinish(LC_ServerPlayer* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry)
{
	if (NULL == pkPlayer || NULL == pkTaskEntry)
	{
		return RE_FAIL;
	}

	bool flag = true;
	//判断该任务是不是日常任务或者公会任务
	//判断该玩家的VIP等级是否可以领两倍奖励
	int taskTitleType = GetTaskTitleType();
	switch(taskTitleType)
	{
	case DAILY_TASK:
		flag = pkPlayer->GetVIPAsset().GetProfitValue(VIP_PROFIT_DAILYTASK_DOUBLE_REWARD)>0 ?  true: false;
		break;
	case GUILD_TASK:
		flag = pkPlayer->GetVIPAsset().GetProfitValue(VIP_PROFIT_GUILDTASK_DOUBLE_REWARD)>0 ?  true: false;
		break;
	default:
		break;
	}

	if(!flag)
	{
		return RE_VIP_LEVEL_LACK;
	}

	//LC_TaskParams& taskParams = pkTaskEntry->GetTaskParam();
	ResultType rslt = RE_SUCCESS;
	do
	{
		if (!m_kParam.IsValid())
		{
			rslt = RE_TASK_PARAM_NOT_VALID;
			break;
		}
		if (m_kParam._iOneKeyShopID <= 0)
		{
			rslt = RE_TASK_NOT_VALID_SHOPID;
			break;
		}
		if (m_kParam._iMultiFinishShopID <= 0)
		{
			rslt = RE_TASK_NOT_VALID_SHOPID;
			break;
		}		
		LC_ShopIDMap kShopIDMap;
		kShopIDMap[m_kParam._iOneKeyShopID] += 1;
		kShopIDMap[m_kParam._iMultiFinishShopID] += 1;
		rslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
		if (rslt != RE_SUCCESS)
		{
			break;
		}
		LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_TASK_FINISH_M, GetTaskID());
		pkPlayer->FinishTaskOP(GetTaskID(), 20000, true);
	} while (0);
	return rslt;
}

void LC_ServerTask::_ProcessControllerLevel(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_LEVEL)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_LEVEL_UP_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int newLevel = (int)pkGameEvent->GetEventParam(EP_LEVEL_UP_VALUE);
		int level = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (level<pkCondition->GetLevel() && newLevel>level)
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, newLevel);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}

void LC_ServerTask::_ProcessPresentBuddyElem(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_PRESENT_BUDDY_ELEM)
		{
			continue;
		}

		int times = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if(times >= 1)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_PRESENT_BUDDYELEM_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}
		
		int conditionNum = pkCondition->GetPresentBuddyElemNum();
		int conditionType = pkCondition->GetPresentBuddyElemType();
		int elemType1 = (int)pkGameEvent->GetEventParam(EP_PRESENT_BUDDYELEM_ELEMTYPE1);
		int elemType2 = (int)pkGameEvent->GetEventParam(EP_PRESENT_BUDDYELEM_ELEMTYPE2);
		int elemType3 = (int)pkGameEvent->GetEventParam(EP_PRESENT_BUDDYELEM_ELEMTYPE3);
		
		int elemNum = 0;	
		elemNum += (elemType1==conditionType);
		elemNum += (elemType2==conditionType);
		elemNum += (elemType3==conditionType);
		
		if (elemNum >= conditionNum)
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, 1);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}

void LC_ServerTask::_ProcessRaidMapTimes(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_RAIDMAP_TIMES)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_RAIDMAP_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		
		int times = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (times < pkCondition->GetRaidMapTimes())
		{
			int addTime = (int)pkGameEvent->GetEventParam(EP_RAIDMAP_TIMES);
			int newTimes = times + addTime;
			newTimes = newTimes>pkCondition->GetRaidMapTimes() ? pkCondition->GetRaidMapTimes() : newTimes;

			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, newTimes);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}

void LC_ServerTask::_ProcessAliveBuddyNum(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_QUIT_ALIVEBUDDYNUM)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_ALIVEBUDDY_MAPID);
		int aliveNum = (int)pkGameEvent->GetEventParam(EP_ALIVEBUDDY_NUM);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int times = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (times<1 && aliveNum>=pkCondition->GetAliveBuddyNum())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, 1);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}

void LC_ServerTask::_ProcessKillBossDropTimes(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_KILLBOSS_DROPTIMES)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_KILLBOSS_DROP_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int times = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (times < pkCondition->GetKillBossDropTimes())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++times);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}

void LC_ServerTask::_ProcessTitleLevel(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_TITLE_LEVEL)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_TITLE_LEVEL_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int level = (int)pkGameEvent->GetEventParam(EP_TITLE_LEVEL);
		int condition = pkCondition->GetTitleLevel();
		int newLevel = level > condition ? condition : level;
	
		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, newLevel);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);

	}
}

void LC_ServerTask::_ProcessAddGuild(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_ADD_GUILD)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_ADD_GUILD_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int times = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (times < 1)
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++times);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}

		//修改后立即同步
		//pkNetPlayer->SyncProcessingTaskMapToClient();
	}
}

void LC_ServerTask::_ProcessTreasureTimes(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}
	
	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_TREASURE_TIMES)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_TREASURE_TIMES_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int times = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (times < pkCondition->GetTreasureTimes())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++times);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}

		//修改后立即同步
		//pkNetPlayer->SyncProcessingTaskMapToClient();
	}
}

void	LC_ServerTask::_ProcessVipExtareReward(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_VIPEXTARE_REWARD)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_VIP_EXTAREREWARD_MAPID);		
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int times = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (times < pkCondition->GetVipQuickHook())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++times);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}

		//修改后立即同步
		//pkNetPlayer->SyncProcessingTaskMapToClient();
	}
}

void LC_ServerTask::_ProcessVIPUpLevel(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_VIPLEVEL)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_VIP_LEVEL_MAPID);		
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int currentLevel = (int)pkGameEvent->GetEventParam(EP_VIP_LEVEL);
		int level = currentLevel > pkCondition->GetVipLevel() ? pkCondition->GetVipLevel() : currentLevel;

		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, level);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);

		//修改后立即同步
		//pkNetPlayer->SyncProcessingTaskMapToClient();
	}
}

void LC_ServerTask::_ProcessChallengeWinEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_CHALLENGE_WIN )
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_CHALLENGE_WIN_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int winTimes = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (winTimes < pkCondition->GetChallengeWinTimes())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++winTimes);

			//修改后立即同步
			pkNetPlayer->SyncProcessingTaskMapToClient();
		}
	}
}

void	LC_ServerTask::_ProcessLoginInEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_LOGININ_DAY )
		{
			continue;
		}

		int taskID = (int)pkGameEvent->GetEventParam(EP_LOGIN_IN_TASKID);
		if(0!=taskID && taskID!=pkTaskEntry->GetTaskID())
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_LOGIN_IN_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int loginInDay = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (loginInDay < pkCondition->GetLoginInDay())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++loginInDay);

			//修改后立即同步
			pkNetPlayer->SyncProcessingTaskMapToClient();
		}
	}
}

void	LC_ServerTask::_ProcessGatherTaskNPCEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_ProcessGatherTaskNPCEvent r1");
		return;
	}

	int iNPCID = (int)pkGameEvent->GetEventParam(EP_GATHERTASK_NPC_CHAR_ID);
	int mapID = (int)pkGameEvent->GetEventParam(EP_GATHERTASK_MAPID);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_ProcessGatherTaskNPCEvent, npcID:%d, mapID:%d", iNPCID, mapID);

	const unique_id_impl& instance = pkPlayer->GetInstance();
	CF_CharType::DataEntry* ct_entry = SafeGetCSVFileDataEntryPtr<CF_CharType>(iNPCID);
	if (NULL == ct_entry)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_ProcessGatherTaskNPCEvent r2 c_cid:%d, c_citizenship:%d", instance.detail(), instance.catagory());
		return;
	}
	
	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_ProcessGatherTaskNPCEvent r3 scheduleID:%d, c_cid:%d, c_citizenship:%d", GetTaskProcessingScheduleID(), instance.detail(), instance.catagory());
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		//if (NULL == pkCondition || pkCondition->GetType() != TFT_GATHER || pkCondition->GetGatherNpc() != iNPCID)
		//{
		//	continue;
		//}

		if (NULL == pkCondition || pkCondition->GetType() != TFT_GATHER)
		{
			continue;
		}

		if(pkCondition->GetGatherNpc() != iNPCID)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_ProcessGatherTaskNPCEvent r4 taskID:%d, conditionNpcID:%d, iNPCID:%d, c_cid:%d, c_citizenship:%d", 
				pkTaskEntry->GetTaskID(), pkCondition->GetGatherNpc(), iNPCID, instance.detail(), instance.catagory());
			continue;
		}

		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_ProcessGatherTaskNPCEvent r5 taskID:%d, mapID:%d, c_cid:%d, c_citizenship:%d", 
				pkTaskEntry->GetTaskID(), mapID, instance.detail(), instance.catagory());
			continue;
		}

		int time = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		int gatherNum = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (gatherNum < pkCondition->GetGatherCount())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++gatherNum);
			//pkNetPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);

			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "add success _ProcessGatherTaskNPCEvent taskID:%d, conditionNum:%d, npcID:%d, mapID:%d, c_cid:%d, c_citizenship:%d", 
				pkTaskEntry->GetTaskID(), gatherNum, iNPCID, mapID, instance.detail(), instance.catagory());
			//采集任务，修改后立即同步
			pkNetPlayer->SyncProcessingTaskMapToClient();
		}
	}
}
void LC_ServerTask::_ProcessKillControllerEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for(int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_KILL_CONTROLLER_NUM)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_KILLCONTROLLER_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int killNum = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (killNum < pkCondition->GetKillControllerNum())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++killNum);
			pkNetPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}
//------------------------------------------------------------------------
void LC_ServerTask::_ProcessTalkToNPCEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (pkCondition && pkCondition->GetType() == TFT_TALK
				&& pkCondition->GetTalkNPC() == pkGameEvent->GetEventParam(EP_TALK_TO_NPC_CHAR_TYPE_ID)
				&& LC_Helper::GetTaskFinishState(pkTaskEntry, i) < 1)
		{

			int mapID = (int)pkGameEvent->GetEventParam(EP_TALK_TO_NPC_MAPID);
			const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
			bool tag = mapList.empty() ? true : false;
			for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
			{
				if(mapID == it->IID())
				{
					tag= true;
					break;
				}
			}

			if(!tag)
			{
				continue;
			}

			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, 1);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}
//------------------------------------------------------------------------
void LC_ServerTask::_ProcessKillNPCEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkGameEvent == NULL || pkTaskEntry == NULL)
	{
		return;
	}

	LC_ServerMapLogic_Place* pkMapLogic = NULL;
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	pkMapLogic = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
	if (pkMapLogic == NULL || pkMapLogic->GetVaild()==false)
	{
		return;
	}

	int iNPCID = (int)pkGameEvent->GetEventParam(EP_KILL_DST_CHAR_TYPE_ID);
	CF_CharType::DataEntry* ct_entry = SafeGetCSVFileDataEntryPtr<CF_CharType>(iNPCID);
	if (NULL == ct_entry)
	{
		return;
	}
	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		//bool bSuccess = false;
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || (pkCondition->GetType() != TFT_MCOUNT && pkCondition->GetType() != TFT_PARTICIPATE_KILL))
		{
			continue;
		}

		int mapID1 = (int)pkGameEvent->GetEventParam(EP_KILL_MAP_RES_ID);
		int mapID2 = (int)pkGameEvent->GetEventParam(EP_RAIDMAP_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID1 == it->IID() || mapID2==it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}		

		const Utility::UT_SSIMDataList & _KillCharTypeID = pkCondition->GetKillCharType();
		if (!_KillCharTypeID.empty())
		{
			Utility::UT_SSIMDataList::const_iterator it = _KillCharTypeID.begin();
			for (; it != _KillCharTypeID.end(); ++it)
			{
				if (it->ID() && it->ID() != (uint32_t)iNPCID)
				{
					continue;
				}

				if (it->ParamA() > pkGameEvent->GetEventParam(EP_KILL_ELITE_LVL))
				{
					continue;
				}

				if (it->ParamB() && !ct_entry->_bBoss)
				{
					continue;
				}

				int iKilledNPC = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
				if (iKilledNPC < pkCondition->GetKillCount())
				{
					kFilter.OnEventHit(this);
					pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iKilledNPC);
					pkNetPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
				}

				if(iKilledNPC >= pkCondition->GetKillCount())
				{
					pkMapLogic->UpdateTaskBatchPlay(pkTaskEntry->GetTaskID());
				}
			}
		}
		else
		{
			int iKilledNPC = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
			if (iKilledNPC < pkCondition->GetKillCount())
			{
				kFilter.OnEventHit(this);
				pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iKilledNPC);
				pkNetPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
			}

			if(iKilledNPC >= pkCondition->GetKillCount())
			{
				pkMapLogic->UpdateTaskBatchPlay(pkTaskEntry->GetTaskID());
			}
		}
	}
}
//------------------------------------------------------------------------
void LC_ServerTask::_ProcessKillGroupNPCEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkGameEvent == NULL || pkTaskEntry == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerMapLogic_Place* pkMapLogic = NULL;
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	pkMapLogic = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
	if (pkMapLogic == NULL || pkMapLogic->GetVaild()==false)
	{
		return;
	}

	int iNPCID = (int)pkGameEvent->GetEventParam(EP_KILL_DST_CHAR_TYPE_ID);
	CF_CharType::DataEntry* ct_entry = SafeGetCSVFileDataEntryPtr<CF_CharType>(iNPCID);
	if (NULL == ct_entry)
	{
		return;
	}
	LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkPlayer;
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_GROUP_KILL)
		{
			continue;
		}
		
		int mapID = (int)pkGameEvent->GetEventParam(EP_KILL_MAP_RES_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		const Utility::UT_SSIMDataList & _KillCharTypeID = pkCondition->GetKillCharType();
		if (!_KillCharTypeID.empty())
		{
			Utility::UT_SSIMDataList::const_iterator it = _KillCharTypeID.begin();
			for (; it != _KillCharTypeID.end(); ++it)
			{
				if (it->ID() && it->ID() != (uint32_t)iNPCID)
				{
					continue;
				}

				int iKilledNPC = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
				if (iKilledNPC < pkCondition->GetKillCount())
				{
					kFilter.OnEventHit(this);
					pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iKilledNPC);
					pkNetPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
				}
			}
		}
	}
}
//------------------------------------------------------------------------
void	LC_ServerTask::_ProcessTaskAreaEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (pkCondition && pkCondition->GetType() == TFT_AREA	&& LC_Helper::GetTaskFinishState(pkTaskEntry, i) < 1)
		{
			const Utility::UT_Vec3Int& kPos = pkServerPlayer->GetCurrentLocation();
			LC_MapBase* pkMap = pkServerPlayer->GetMap();
			if (pkMap == NULL)
			{
				continue;
			}

			int mapID = (int)pkGameEvent->GetEventParam(EP_ARRIVE_AREA_MAP_ID);
			const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
			bool tag = mapList.empty() ? true : false;
			for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
			{
				if(mapID == it->IID())
				{
					tag= true;
					break;
				}
			}


			if(!tag)
			{
				continue;
			}

			int iMapID = pkServerPlayer->GetMapResID();
			int type = 0;
			StringType name = "not define";
			if (LC_LogicTriggerManager::GetSingletonPtr()->QueryTriggerInfo(pkCondition->GetArea(), kPos, iMapID, type, name))
			{
				kFilter.OnEventHit(this);
				pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, 1);
				pkServerPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
			}
		}
	}
}
//------------------------------------------------------------------------
void	LC_ServerTask::_ProcessPickItemEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	return;
}
//------------------------------------------------------------------------
void	LC_ServerTask::_ProcessTimeClickEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntr, LC_TaskEventFilter& kFiltery)
{

}
//------------------------------------------------------------------------
void	LC_ServerTask::_ProcessNPCArriveEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
}
//------------------------------------------------------------------------
void	LC_ServerTask::_ProcessUseItemEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{

}

void LC_ServerTask::_ProcessSkillExecuteEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_EXECUTE_SKILL)
		{
			continue;
		}
		int lSkillType =  (int)pkGameEvent->GetEventParam(EP_SKILL_SKILL_EXECUTE_SKILL_ID) ;
		if (lSkillType < pkCondition->GetSkillStart() || lSkillType > pkCondition->GetSkillEnd())
		{
			continue;
		}
		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount < pkCondition->GetSkillEnd())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iCount);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}
void LC_ServerTask::_ProcessTranscriptionEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	int lEventType =  (int)pkGameEvent->GetEventParam(EP_TRANSCRIPTION_EVENT_TYPE);
	int lTransID =  (int)pkGameEvent->GetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_2);
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_RAID)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetRaidCount())
		{
			continue;
		}
		const Utility::UT_SSIMDataListInt16 & _Raids = pkCondition->GetRaids();
		Utility::UT_SSIMDataListInt16::const_iterator it = _Raids.begin();
		for (; it != _Raids.end(); ++it)
		{
			/*
			if (it->ID() && it->ID() != (uint32_t)lTransID)
			{
				continue;
			}
			if (it->ParamA() != lEventType)
			{
				continue;
			}
			*/
			if (it->GetDataList0() && it->GetDataList0() != (uint32_t)lTransID)
			{
				continue;
			}
			if (it->GetDataList1() != lEventType)
			{
				continue;
			}
			if (iCount >= pkCondition->GetRaidCount())
			{
				break;
			}
			kFilter.OnEventHit(this);
			++iCount;
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}
void LC_ServerTask::_ProcessFinishTaskEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}
	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}
	int iCatID = (int)pkGameEvent->GetEventParam(EP_FINISH_TASK_CAT) ;
	int iTaskID = (int)pkGameEvent->GetEventParam(EP_FINISH_TASK_ID);
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_TASK)
		{
			continue;
		}
		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetTaskCount())
		{
			continue;
		}
		const Utility::UT_SSIMDataList & _Tasks = pkCondition->GetTasks();
		if (_Tasks.empty())
		{
			if (iCatID != pkCondition->GetTaskCat())
			{
				continue;
			}
			kFilter.OnEventHit(this);
			iCount += (int)pkGameEvent->GetEventParam(EP_FINISH_TASK_CNT);
		}
		else
		{
			Utility::UT_SSIMDataList::const_iterator it = _Tasks.begin();
			for (; it != _Tasks.end(); ++it)
			{
				if (it->ID() && it->ID() != (uint32_t)iTaskID)
				{
					continue;
				}
				if (iCount >= pkCondition->GetTaskCount())
				{
					break;
				}
				kFilter.OnEventHit(this);
				iCount += (int)pkGameEvent->GetEventParam(EP_FINISH_TASK_CNT);
			}
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessStoryProcessEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_STORY)
		{
			continue;
		}
		int iStoryID = (int)pkGameEvent->GetEventParam(EP_STORY_PROCESS_STORY_ID) ;
		if (iStoryID != pkCondition->GetStoryID())
		{
			continue;
		}

		bool tag = false;
		int mapID = (int)pkGameEvent->GetEventParam(EP_STORY_PROCESS_MAPID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		

		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount < pkCondition->GetStorySection())
		{
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iCount);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
}

void LC_ServerTask::_ProcessChallengeEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || (pkCondition->GetType() != TFT_CHALLENGE && pkCondition->GetType() != TFT_CROSS_ARENA_CHALLENGE)  )
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_CHALLENGE_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}


		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetChallengeCount())
		{
			continue;
		}
		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void	LC_ServerTask::_ProcessTotalAddCashEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_TOTAL_ADD_CASH)
		{
			continue;
		}

		if (pkCondition->GetAddCashType() != pkGameEvent->GetEventParam(EP_TOTAL_ADD_CASH_TYPE))
		{
			continue;
		}
		const Utility::UT_SIMDataMap & _AddCashSrc = pkCondition->GetAddCashSrc();
		if (!_AddCashSrc.empty())
		{
			int32_t iSrcType = (int)pkGameEvent->GetEventParam(EP_TOTAL_ADD_CASH_SRC_TYPE);
			if (_AddCashSrc.find(iSrcType) == _AddCashSrc.end())
			{
				continue;
			}
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_TOTAL_ADD_CASH_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetAddCash())
		{
			continue;
		}
		iCount = UT_MathBase::MinInt(pkCondition->GetAddCash(), (int)pkGameEvent->GetEventParam(EP_TOTAL_ADD_CASH_COUNT));
		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessAddCashEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_CASHADD)
		{
			continue;
		}

		if (pkCondition->GetAddCashType() != pkGameEvent->GetEventParam(EP_ADD_CASH_TYPE))
		{
			continue;
		}
		const Utility::UT_SIMDataMap & _AddCashSrc = pkCondition->GetAddCashSrc();
		if (!_AddCashSrc.empty())
		{
			int32_t iSrcType = (int)pkGameEvent->GetEventParam(EP_ADD_CASH_SRC_TYPE);
			if (_AddCashSrc.find(iSrcType) == _AddCashSrc.end())
			{
				continue;
			}
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_ADD_CASH_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}


		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetAddCash())
		{
			continue;
		}
		iCount = UT_MathBase::MinInt(pkCondition->GetAddCash(), iCount + (int)pkGameEvent->GetEventParam(EP_ADD_CASH_COUNT));
		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessReduceCashEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}
	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_CASHREDUCE)
		{
			continue;
		}
		if (pkCondition->GetReduceCashType() != pkGameEvent->GetEventParam(EP_REDUCE_CASH_TYPE))
		{
			continue;
		}
		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetReduceCash())
		{
			continue;
		}
		if (pkGameEvent->GetEventParam(EP_REDUCE_CASH_SRC_TYPE) == SYS_FUNC_TP_TRADE)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_REDUCE_CASH_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}
		
		iCount = UT_MathBase::MinInt(pkCondition->GetReduceCash(), iCount + (int)pkGameEvent->GetEventParam(EP_REDUCE_CASH_COUNT));
		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessSPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_SP)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetSPGot())
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_SP_ADD_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		iCount = UT_MathBase::MinInt(pkCondition->GetSPGot(), iCount + (int)pkGameEvent->GetEventParam(EP_SP_ADD_COUNT));
		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessComboEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}
	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_COMBO)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetCombo())
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_COMBO_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}
		iCount = std::min(pkCondition->GetCombo(), (int)pkGameEvent->GetEventParam(EP_COMBO_COUNT));
		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessItemIdentifyEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_IDENTIFY)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetItemIdentify())
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_ITEM_IDENTIFY_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		kFilter.OnEventHit(this);
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}
void  LC_ServerTask::_ProcessKillEliteEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
}

void LC_ServerTask::_ProcessItemGotEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	int iReason = (int)pkGameEvent->GetEventParam(EP_ADD_REASON);
	//int iItemID = (int)pkGameEvent->GetEventParam(EP_ADD_ITEM_ID);
	//int iItemNum = (int)pkGameEvent->GetEventParam(EP_ADD_ITEM_PACK_NUM);

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; ++i)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_ITEM_GOT)
		{
			continue;
		}
		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetItemGotCount())
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_ADD_ITEM_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		const mem::map<int32_t, int32_t>& itemData = pkGameEvent->GetKeyBaseData();
		const Utility::UT_SSIMDataList & _ItemGot = pkCondition->GetItemGot();
		Utility::UT_SSIMDataList::const_iterator iter = _ItemGot.begin();
		for (; iter != _ItemGot.end(); ++iter)
		{
			for(mem::map<int32_t, int32_t>::const_iterator kIt=itemData.begin(); kIt!=itemData.end(); ++kIt)
			{
				int32_t iItemID = kIt->first;
				int32_t iItemNum = kIt->second;

				if (iter->ID() && iter->ID() != (uint32_t)iItemID)
				{
					continue;
				}
				if (iter->ParamA() && iter->ParamA() != iReason)
				{
					continue;
				}
				if (iCount >= pkCondition->GetItemGotCount())
				{
					break;
				}
				kFilter.OnEventHit(this);
				iCount += iItemNum;
			}
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessItemSwitchEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	int iSrcItemID = (int)pkGameEvent->GetEventParam(EP_SWITCH_ITEM_SRC_ID);
	int iSrc = (int)pkGameEvent->GetEventParam(EP_SWITCH_ITEM_SRC_PACK_TYPE);
	int iDstItemID = (int)pkGameEvent->GetEventParam(EP_SWITCH_ITEM_DST_ID);
	int iDst = (int)pkGameEvent->GetEventParam(EP_SWITCH_ITEM_DST_PACK_TYPE);

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_ITEM_SWITCH)
		{
			continue;
		}
		if (LC_Helper::GetTaskFinishState(pkTaskEntry, i) >= 1)
		{
			continue;
		}
		const Utility::UT_SSIMDataList& _ItemSwitch = pkCondition->GetItemSwitch();
		Utility::UT_SSIMDataList::const_iterator it = _ItemSwitch.begin();
		for (; it != _ItemSwitch.end(); ++it)
		{
			if (it->ID())
			{
				if (it->ID() == (uint32_t)iSrcItemID)
				{
					if (it->ParamA() != iSrc || it->ParamB() != iDst)
					{
						continue;
					}
				}
				else if (it->ID() == (uint32_t)iDstItemID)
				{
					if (it->ParamB() != iSrc || it->ParamA() != iDst)
					{
						continue;
					}
				}
				else
				{
					continue;
				}
			}
			else
			{
				if ((it->ParamA() != iSrc || it->ParamB() != iDst) && (it->ParamB() != iSrc || it->ParamA() != iDst))
				{
					continue;
				}
			}
			kFilter.OnEventHit(this);
			pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, 1);
			break;
		}
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessCommonStatOPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	int iType = (int)pkGameEvent->GetEventParam(EP_CSOP_TYPE);
	//int iKey = pkGameEvent->GetEventParam(EP_CSOP_KEY);
	int iOPFlag = (int)pkGameEvent->GetEventParam(EP_CSOP_OP_FLAG);

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_COMMON_STAT)
		{
			continue;
		}
		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetCommonStatCount())
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_CSOP_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		const Utility::UT_SSIMDataList & _CommonStats = pkCondition->GetCommonStats();
		Utility::UT_SSIMDataList::const_iterator it = _CommonStats.begin();
		for (; it != _CommonStats.end(); ++it)
		{
			if (it->ID() && iType != (int)it->ID())
			{
				continue;
			}

			if ((it->ParamA() & iOPFlag) == 0)
			{
				continue;
			}

			if (iCount >= pkCondition->GetCommonStatCount())
			{
				continue;
			}
			kFilter.OnEventHit(this);
			++iCount;
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}
void LC_ServerTask::_ProcessActiStatOPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	int iActiType = (int)pkGameEvent->GetEventParam(EP_ACTOP_ACTITYPE);
	int iCategory = (int)pkGameEvent->GetEventParam(EP_ACTOP_CATEGORY);
	int iOPCode = (int)pkGameEvent->GetEventParam(EP_ACTOP_OPCODE);
	int iCount =  (int)pkGameEvent->GetEventParam(EP_ACTOP_COUNT);

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_ACTI_STAT)
		{
			continue;
		}
		int iFinishCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		int iConditionCount = pkCondition->GetActStatCount();
		if (iFinishCount >= pkCondition->GetActStatCount())
		{
			continue;
		}
		
		int mapID = (int)pkGameEvent->GetEventParam(EP_ACTOP_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		const Utility::UT_SSIMDataList & _ActStats = pkCondition->GetActStats();
		Utility::UT_SSIMDataList::const_iterator it = _ActStats.begin();
		for (; it != _ActStats.end(); ++it)
		{
			if (iActiType != (int)it->ID())
			{
				continue;
			}
			if (it->ParamA() != iCategory)
			{
				continue;
			}
			if (it->ParamB() != iOPCode)
			{
				continue;
			}
			if (iFinishCount >= iConditionCount)
			{
				continue;
			}
			kFilter.OnEventHit(this);
			int totalCount = iFinishCount+iCount;
			iFinishCount = totalCount>iConditionCount ? iConditionCount : totalCount;
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iFinishCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}
void LC_ServerTask::_ProcessTreasureChestOPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	//int iOPID = pkGameEvent->GetEventParam(EP_TCOP_OP_ID);
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_TREASURE_CHEST)
		{
			continue;
		}
	
		int mapID = (int)pkGameEvent->GetEventParam(EP_TCOP_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetTreasureChestCount())
		{
			continue;
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessSigninEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_SIGNIN)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_SIGNIN_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetSignin())
		{
			continue;
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}
void LC_ServerTask::_ProcessGuildOfferEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_GUILD_OFFER)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_GUILD_OFFER_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetGuildOffer())
		{
			continue;
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, ++iCount);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ServerTask::_ProcessGuildKillEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	int iTargetTotal = (int)pkGameEvent->GetEventParam(EP_GUILD_ECT_TOTAL_LOW);
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_GUILD_KILL)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_GUILD_ECT_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		if (iCount >= pkCondition->GetGuildKill())
		{
			continue;
		}
		pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iTargetTotal);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}
void LC_ServerTask::_ProcessItemOperationEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	int32_t iPacketType = (int)pkGameEvent->GetEventParam(EP_ITEM_OPERATION_PACK_TP);
	int32_t iPacketIndex = (int)pkGameEvent->GetEventParam(EP_ITEM_OPERATION_PACK_IDX);
	int32_t iOPCode = (int)pkGameEvent->GetEventParam(EP_ITEM_OPERATION_PACK_OP);

	LC_BackPackEntry* pkEntry = pkPlayer->GetPackAsset()->GetBackPackEntry(iPacketType, iPacketIndex);
	if (NULL == pkEntry || !pkEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return;
	}

	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_ITEM_OPERATION)
		{
			continue;
		}

		int mapID = (int)pkGameEvent->GetEventParam(EP_ITEM_OPERATION_MAP_ID);
		const Utility::UT_SIMDataList& mapList = pkCondition->GetMapID();
		bool tag = mapList.empty() ? true : false;
		for(Utility::UT_SIMDataList::const_iterator it=mapList.begin(); it!=mapList.end(); ++it)
		{
			if(mapID == it->IID())
			{
				tag= true;
				break;
			}
		}

		if(!tag)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		const Utility::UT_SSIMDataList & _ItemOperations = pkCondition->GetItemOperations();
		Utility::UT_SSIMDataList::const_iterator it = _ItemOperations.begin();
		for (; it != _ItemOperations.end(); ++it)
		{
			int32_t iCur = 0;
			if (it->ID() == (uint32_t)iOPCode)
			{
				if (iCount >= it->ParamA())
				{
					continue;
				}
				switch (iOPCode)
				{
					case LC_ITEM_OP_CODE_EXCHANGE:
						{
							iCur = iCount + 1;
						}
						break;
					default:
						break;
				}
				if (iCur > iCount)
				{
					pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iCur);
					pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
				}
			}
		}

	}
}

void LC_ServerTask::_ProcessSkillLevelUPEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_ProcessingTaskEntry* pkTaskEntry, GameLogic::LC_TaskEventFilter& kFilter)
{
	if (pkPlayer == NULL || pkTaskEntry == NULL || pkGameEvent == NULL)
	{
		return;
	}

	if(false == UT_ServerHelper::CheckActivityVaild(GetTaskProcessingScheduleID()))
	{
		return;
	}

	//skill_id_type iSkillTypeID = pkGameEvent->GetEventParam(EP_SKILL_LEVEL_UP_SKILL_ID);
	int32_t iSkillLevel = (int)pkGameEvent->GetEventParam(EP_SKILL_LEVEL_UP_LEVEL);
	for (int i = 0 ; i < MAX_FINISH_CONDITION_COUNT; i++)
	{
		LC_Task_Condition* pkCondition = GetFinishCondition(i, &pkTaskEntry->GetTaskParam());
		if (NULL == pkCondition || pkCondition->GetType() != TFT_SKILL_LEVEL)
		{
			continue;
		}

		int iCount = LC_Helper::GetTaskFinishState(pkTaskEntry, i);
		const Utility::UT_SSIMDataList & _SkillLevels = pkCondition->GetSkillLevels();
		Utility::UT_SSIMDataList::const_iterator it = _SkillLevels.begin();
		for (; it != _SkillLevels.end(); ++it)
		{
			if (it->ID() <= 0)
			{
				if (iSkillLevel > iCount)
				{
					pkPlayer->GetTaskMap()->SetFinishState(this, pkTaskEntry, i, iSkillLevel);
					pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
				}
			}
		}
	}
}
void LC_ServerTask::_ProcessPlayerDieEvent(GameLogic::LC_GameEvent* pkGameEvent, GameLogic::LC_PlayerBase* pkPlayer, LC_ProcessingTaskEntry* pkTaskEntry, LC_TaskEventFilter& kFilter)
{

}

//------------------------------------------------------
void LC_ServerTask::_handleBonusItem(GameLogic::LC_PlayerBase* pkPlayer, int32_t iMulti, int32_t addExtraReward)
{
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (NULL == pkItemFactory)
	{
		return;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return;
	}

	Utility::UT_SIMDataList kItems;
	{
		LC_ItemsRewardList::iterator it = m_kItemRewards.begin();
		for (; it != m_kItemRewards.end(); ++it)
		{
			it->Resolve(pkPlayer, kItems, NULL);
		}
	}

	if(addExtraReward > 0)
	{
		LC_ItemsRewardList::iterator it = m_kExtraItemRewards.begin();
		for (; it != m_kExtraItemRewards.end(); ++it)
		{
			it->Resolve(pkPlayer, kItems, NULL);
		}
	}
	else
	{
		pkPlayer->AddTaskExtraReward(m_kExtraItemRewards);
	}

	StringType title;
	std::stringstream ss;
	ss << GetTaskID() << ";";
	switch (m_iProperty_TaskType)
	{
		case STT_DAILY_TASK:
			title = "192;" + STDSTR2TPSTR(ss.str());
			break;
		case STT_GUILD_TASK:
			title = "193;" + STDSTR2TPSTR(ss.str());
			break;
		case STT_GAMESTORY:
			title = "194;" + STDSTR2TPSTR(ss.str());
			break;
		case STT_CRUSADE_TOKEN:
			title = "195;" + STDSTR2TPSTR(ss.str());
			break;
		case STT_MAIN_SCENARIO:
			title = "196;" + STDSTR2TPSTR(ss.str());
			break;
		case STT_REWARD_TASK:
			title = "197;" + STDSTR2TPSTR(ss.str());
			break;
		default:
			title = "198;" + STDSTR2TPSTR(ss.str());
			break;
	}
	//任务完成--埋点日志
	int nLogCode = 0;
	vector<int> nParams;
	nParams.clear();
	int nTaskId = GetTaskID();
	nParams.push_back(nTaskId);
	switch(m_iProperty_TaskType)
	{
	case STT_MAIN_SCENARIO:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MAINTASK_OPERATE, OPERATE_LOGTYPE1);
		}break;
	case STT_GUILD_TASK:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, GUILDTASK_OPERATE, OPERATE_LOGTYPE1);
		}break;
	case STT_DAILY_TASK:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, DAILYTASK_OPERATE, OPERATE_LOGTYPE1);
		}break;
	case STT_REWARD_TASK:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, OFFERAWARD_OPERATE, OPERATE_LOGTYPE1);
		}break;
	case STT_BUDDYTHEMETASK:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BUDDYTASK_OPERATE, OPERATE_LOGTYPE1);
		}break;
	case STT_EVERYDAY_TASK:
		{
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, LEADERTASK_OPERATE, OPERATE_LOGTYPE1);
		}break;
	}
	UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_TASK_FINISH, GetTaskID(), title, "199;",true,false, false, nLogCode,nParams);
}

