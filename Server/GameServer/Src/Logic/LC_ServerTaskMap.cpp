#include "LM_LocaleManager.h"
#include "LC_ServerTaskMap.h"
#include "LC_ServerTask.h"
#include "LC_ServerTaskManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "UT_Log.h"
#include "LC_GameEventManager.h"
#include "CF_DailyLiveness.h"

#include "UT_Timer.h"

#include "TS_TranManager.h"
// #include "TS_TranType.h"
// #include "LC_SequenceManager.h"

#include "UT_ServerHelper.h"

using namespace GameLogic;
using namespace Utility;
using namespace CSVFile;
using namespace Protocol;

LC_ServerTaskMap::LC_ServerTaskMap()
{
	Clear();
}

LC_ServerTaskMap::~LC_ServerTaskMap()
{
	Clear();
}

void LC_ServerTaskMap::CheckReset(uint32_t timestamp, int32_t iTaskType, bool bforce, bool bKeepProcessing)
{
	LC_ServerTaskManager* pkTaskMgr = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();

	LC_AssignTaskInfo* pkAssignInfo = GetAssignTaskInfo();
	uint32_t uiAssignType = pkTaskMgr->GetAssignType(iTaskType);

	bool bTaskTypeReset = false;
	LC_TaskTypeData* pkType = GetTypeData(iTaskType);
	if (pkType && pkType->Reset(timestamp))
	{
		bTaskTypeReset = true;
		UpdateDirtyFlag(DIRTY_FLAG_TASK_TYPE_DATA);
	}
	
	bool nreset = pkAssignInfo->CheckReset(uiAssignType, timestamp, bforce, bKeepProcessing);

	switch(iTaskType)
	{
	case STT_DAILY_TASK:
	case STT_GUILD_TASK:
	case STT_FRONTIER:
		{
			if (nreset)
			{
				ClearAllTaskRecordOfType(iTaskType, bKeepProcessing);
			}
		}
		break;
	case STT_MONOPOLY_TASK:
		{
			if (bTaskTypeReset)
			{
				ClearAllTaskRecordOfType(iTaskType, bKeepProcessing, true);
			}
		}
		break;
	case STT_DAILY_CHALLENGE:
		{
			LC_LivenessEntry& entry = GetLivenessEntry(iTaskType);
			if(!entry.Reset(timestamp))
			{
				return;
			}
			ClearAllTaskRecordOfType(iTaskType, bKeepProcessing, true);
		}
		break;
	default:
		break;
	}
}

void LC_ServerTaskMap::ClearAllTaskRecordOfType(int32_t iTaskType, bool bKeepProcessing, bool bResetProcessing)
{
	LC_ServerTaskManager* pkTaskMgr = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	const TaskIDVec& theTasks = pkTaskMgr->GetTaskIDs(iTaskType);

	TaskIDVec::const_iterator it = theTasks.begin();
	for (; it!=theTasks.end(); ++it)
	{
		RemoveTaskFinishRecord(*it);
		
		//重置日常任务的时候，把未完成的日常任务取消掉，玩家重新接
		//if (!bKeepProcessing || iTaskType==STT_DAILY_TASK)
		if (!bKeepProcessing)
		{
			LC_ProcessingTaskEntryMap::iterator itP = m_kProcessingTaskMap.find(*it);
			if (itP != m_kProcessingTaskMap.end())
			{
				m_pkOwner->CancelTask(itP->second.GetTaskID());
			}
		}
		else if (bResetProcessing)
		{
			LC_ProcessingTaskEntryMap::iterator itP = m_kProcessingTaskMap.find(*it);
			if (itP != m_kProcessingTaskMap.end())
			{
				itP->second.ResetProcessingState();
				UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
			}
		}
	}
}

void LC_ServerTaskMap::Clear()
{
	LC_TaskMap::Clear();
}

ResultType LC_ServerTaskMap::DrawLevelReward(uint32_t iTaskType, uint32_t level)
{

	if (iTaskType != STT_DAILY_CHALLENGE)
	{
		return RE_TASK_SYSTEM_ERROR;
	}

	if (m_kDailyLiveness.IsDrawed(level))
	{
		return RE_TASK_DAILY_LIVENESS_LVL_DRAWED;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();

	LC_DailyLivenessEntry* pkEntry = pkTaskManager->GetDailyLivenessEntry(level);

	if (NULL==pkEntry)
	{
		return RE_TASK_DAILY_LIVENESS_BAD_LVL;
	}

	ResultType rslt = RE_SUCCESS;
	do
	{		
		if(!pkEntry->_kGifts.empty())
		{
			UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkEntry->_kGifts, m_pkOwner, SYS_FUNC_TP_TASK_LIVENESS, iTaskType, "204;", "204;");
		}

		
		int plvl = m_pkOwner->GetLevel();

		if (pkEntry->GetExp(plvl))
		{
			m_pkOwner->AddExpWithGamePlay(pkEntry->GetExp(plvl), SYS_FUNC_TP_TASK_LIVENESS, iTaskType);
		}

		if (pkEntry->GetGold(plvl))
		{
			m_pkOwner->AddCash(pkEntry->GetGold(plvl), CT_UNBIND_CASH, SYS_FUNC_TP_TASK_LIVENESS, iTaskType);
		}


		if (pkEntry->GetSp(plvl))
		{
			m_pkOwner->AddSkillLevelupPoints(pkEntry->GetSp(plvl), SYS_FUNC_TP_TASK_LIVENESS, iTaskType);
		}


		m_kDailyLiveness.RegDrawLevel(level);

		UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}while (0);

	return rslt;
}

class AssignOneKeyCBArg : public TS_PostCommit
{
public:
	uint32_t	m_uiCharID;
	LC_TaskAssignEntry *m_pkTaskAssignEntry;
	int	m_iCount;

	AssignOneKeyCBArg(uint32_t uiCharID, LC_TaskAssignEntry* pkTaskAssignEntry, int iCount)
		:m_uiCharID(uiCharID)
		,m_pkTaskAssignEntry(pkTaskAssignEntry)
		,m_iCount(iCount)
	{}

	virtual void operator()(void)
	{
		uint32_t uiTaskAssignType = m_pkTaskAssignEntry->_iID;
		LC_ServerPlayer * pkPlayer = (LC_ServerPlayer *)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(m_uiCharID);
		if(NULL == pkPlayer)
		{
			return;
		}

		LC_TaskManager* pkTaskMgr = LC_TaskManager::GetSingletonPtr();

		LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();

		LC_AssignTaskInfo* pkTaskAssignInfo = pkTaskMap->GetAssignTaskInfo();
		LC_AssignTaskData* pkTaskAssign = pkTaskAssignInfo->GetAssignTaskData(uiTaskAssignType);
		LC_TaskTypeDataEntry* pkTypeDataEntry = pkTaskMgr->GetTaskTypeDataEntry(m_pkTaskAssignEntry->_iTaskType);

		int32_t iTaskID = 0;
		const TaskIDSet& kTaskIDs = pkTaskAssign->GetAssignTaskIDs();
		if (!kTaskIDs.empty())
		{
			iTaskID = *kTaskIDs.begin();
		}
		
		pkTaskAssignInfo->SetRound(uiTaskAssignType, m_pkTaskAssignEntry->_iMaxRound);
		pkTaskMap->ClearAllTaskRecordOfType(m_pkTaskAssignEntry->_iTaskType, false);
		pkTaskAssignInfo->ClearAssignTask(uiTaskAssignType);

		int playerLevel = pkPlayer->GetLevel();

		attr_value_type iExp = m_pkTaskAssignEntry->GetOneKeyExp(playerLevel, m_iCount);
		if (iExp)
		{
			pkPlayer->AddExpWithGamePlay(iExp, SYS_FUNC_TP_TASK_FINISH, m_pkTaskAssignEntry->_iTaskType);
		}

		MoneyType iCash = m_pkTaskAssignEntry->GetOneKeyCash(playerLevel, m_iCount);
		if (iCash)
		{
			pkPlayer->AddCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_TASK_FINISH, m_pkTaskAssignEntry->_iTaskType);
		}

		attr_value_type iSp = m_pkTaskAssignEntry->GetOneKeySp(playerLevel, m_iCount);
		if (iSp)
		{
			pkPlayer->AddSkillLevelupPoints(iSp, SYS_FUNC_TP_TASK_FINISH, m_pkTaskAssignEntry->_iTaskType);
		}

		MoneyType iGuild = m_pkTaskAssignEntry->GetOneKeyGuild(playerLevel, m_iCount);
		if (iGuild)
		{
			pkPlayer->AddGuildMoney(iGuild, SYS_FUNC_TP_TASK_FINISH, m_pkTaskAssignEntry->_iTaskType);
		}

		pkTaskMap->DrawAssignRoundOffReward(uiTaskAssignType);

		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_FINISH_TASK);
			pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,	GENT_SELF);
			pkGameEvent->SetEventParam(EP_FINISH_TASK_PLAYER_ID, m_uiCharID);
			pkGameEvent->SetEventParam(EP_FINISH_TASK_ID, iTaskID);
			pkGameEvent->SetEventParam(EP_FINISH_TASK_CAT, m_pkTaskAssignEntry->_iTaskType);
			pkGameEvent->SetEventParam(EP_FINISH_TASK_CNT, m_iCount);

			pkEventManager->InsertGameEvent(pkGameEvent);
		}

		for(TaskIDSet::const_iterator citTID=kTaskIDs.begin(); citTID!=kTaskIDs.end(); ++citTID)
		{
			pkPlayer->GetOpenAbilityAsset().OnTaskFinished(*citTID);
		}

		if (pkTypeDataEntry  )
		{
			LC_CommonStatAsset& kCommonStatAsset = pkPlayer->GetCommonStatAsset();
			LC_CommonStatEntry* pkCommonStatEntry = kCommonStatAsset.GetCommonStatEntry(pkTypeDataEntry->_iCommonStatID);
			if (pkCommonStatEntry)
			{
				pkCommonStatEntry->SetStatEntry(LC_CMN_STAT_ENTRY_KEY_DAY_FINISH, pkCommonStatEntry->GetStatEntry(LC_CMN_STAT_ENTRY_KEY_DAY_FINISH)+m_iCount);
			}
		}

		AchievementEvent kEvent;
		kEvent.SetConditionInfo(m_pkTaskAssignEntry->_iTaskType);
		kEvent.SetNum(1);
		pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_FINISH_TASK_COUNT, kEvent);
	}
};

ResultType LC_ServerTaskMap::OneKeyOperation(uint32_t iTaskAssignType)
{
	LC_AssignTaskInfo* pkTaskAssignInfo = GetAssignTaskInfo();
	LC_AssignTaskData* pkTaskAssign = pkTaskAssignInfo->GetAssignTaskData(iTaskAssignType);
	if (NULL == pkTaskAssign)
	{
		return RE_FAIL;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();

	if (NULL == pkTaskManager)
	{
		return RE_FAIL;
	}
		
	LC_TaskAssignEntry *pkEntry = pkTaskManager->GetTaskAssignEntry(iTaskAssignType);
	if (NULL == pkEntry)
	{
		return RE_FAIL;
	}

	ResultType rslt = RE_SUCCESS;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)m_pkOwner;

	do
	{
		int iCount = pkEntry->_iMaxRound - pkTaskAssign->GetRound();
		iCount += pkTaskAssign->GetAssignTaskCount()>0 ? 1 : 0;
		if (iCount<=0) 
		{
			return RE_TASK_MAX_ASSIGN_ROUND;
		}

		if (pkEntry->_iOneKeyShopID <= 0)
		{
			rslt = RE_TASK_NOT_VALID_SHOPID;
			break;
		}

		LC_ShopIDMap kShopIDMap;
		kShopIDMap[pkEntry->_iOneKeyShopID] = iCount;
		ResultType rslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
		if (rslt != RE_SUCCESS)
		{
			break;
		}

		LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_TASK_ONEKEY_TYPE, pkEntry->_iTaskType);

		{
			AssignOneKeyCBArg kArg(pkPlayer->GetID(), pkEntry, iCount);
			kArg();
		}

	} while (0);



	return RE_SUCCESS;
}
class PhagotrophyCBArg : public TS_PostCommit
{
public:
	uint32_t	m_uiCharID;
	GameLogic::LC_Task* m_pkPhagotrophTask;
	GameLogic::LC_Task* m_pkPhagocytableTask;
	float m_fPropo;

	PhagotrophyCBArg(uint32_t uiCharID, GameLogic::LC_Task* pkPhagotrophTask, GameLogic::LC_Task* pkPhagocytableTask, float propo)
		:m_uiCharID(uiCharID)
		,m_pkPhagotrophTask(pkPhagotrophTask)
		,m_pkPhagocytableTask(pkPhagocytableTask)
		,m_fPropo(propo)
	{}

	virtual void operator()(void)
	{
		LC_ServerPlayer * pkPlayer = (LC_ServerPlayer *)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(m_uiCharID);
		if(NULL==pkPlayer)
		{
			return;
		}

		LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();

		pkTaskMap->Phagotrophy(m_pkPhagotrophTask, m_pkPhagocytableTask, m_fPropo);
	}
};
ResultType LC_ServerTaskMap::PhagotrophyOperation(GameLogic::LC_Task* pkPhagotrophTask, GameLogic::LC_Task* pkPhagocytableTask, int iType, float& propo)
{
	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();

	if (NULL == pkTaskManager)
	{
		return RE_FAIL;
	}

	if (NULL==pkPhagotrophTask || NULL==pkPhagocytableTask)
	{
		return RE_FAIL;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return RE_FAIL;
	}

	ResultType rslt = RE_SUCCESS;

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)m_pkOwner;

	do
	{
		rslt = CanPhagotrophy(pkPhagotrophTask, pkPhagocytableTask);
		if (RE_SUCCESS != rslt)
		{
			break;
		}

		const LC_TaskParamEntry& kTaskParamEntry = pkPhagocytableTask->GetTaskParam();
		if(!kTaskParamEntry.IsValid())
		{
			rslt = RE_TASK_PARAM_NOT_VALID;
			break;
		}

		propo = 1.0;

		if(iType == 0)
		{
			if(!pkPlayer->IsCashEnough(kTaskParamEntry._iPhagotrophyShopID, CT_UNBIND_CASH))
			{
				rslt = RE_TASK_PHAGOTROPHY_CASH_LACK;
				break;
			}

			propo = UT_MathBase::RandFloat01()*0.3f + 0.5f;
			
			PhagotrophyCBArg kArg = PhagotrophyCBArg(pkPlayer->GetID(), pkPhagotrophTask, pkPhagocytableTask, propo);
			kArg();
		}
		else
		{
			if (kTaskParamEntry._iPhagotrophyShopID<=0)
			{
				rslt = RE_TASK_NOT_VALID_SHOPID;
				break;
			}

			LC_ShopIDMap kShopIDMap;
			kShopIDMap[kTaskParamEntry._iPhagotrophyShopID] = 1;
			rslt = LC_Helper::IsShopAvailable(pkPlayer, kShopIDMap);
			if (rslt != RE_SUCCESS)
			{
				break;
			}

			LC_Helper::PayForShop(pkPlayer, kShopIDMap, SYS_FUNC_TP_TASK_PHAGOT, pkPhagocytableTask->GetTaskID());

			{
				PhagotrophyCBArg kArg = PhagotrophyCBArg(pkPlayer->GetID(), pkPhagotrophTask, pkPhagocytableTask, propo);
				kArg();
			}
		}

	} while (0);


	return rslt;
}
ResultType LC_ServerTaskMap::DrawAssignRoundOffReward(uint32_t iTaskAssignType)
{
	LC_AssignTaskInfo* pkTaskAssignInfo = GetAssignTaskInfo();
	LC_AssignTaskData* pkTaskAssign = pkTaskAssignInfo->GetAssignTaskData(iTaskAssignType);
	if (NULL == pkTaskAssign)
	{
		return RE_FAIL;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	LC_TaskAssignEntry *pkEntry = pkTaskManager->GetTaskAssignEntry(iTaskAssignType);
	if (NULL == pkEntry)
	{
		return RE_FAIL;
	}

	ResultType rslt = RE_SUCCESS;
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)m_pkOwner;
	int playerLevel = pkPlayer->GetLevel();

	do
	{
		int iCount = pkEntry->_iMaxRound - pkTaskAssign->GetRound();
		if (iCount > 0) 
		{
			rslt = RE_TASK_ASSIGN_ROUND_NT_OFF;
			break;
		}

		if (pkTaskAssign->Rewarded())
		{
			rslt = RE_TASK_ASSIGN_ROUND_REWARDED;
			break;
		}
		
		attr_value_type iExp = pkEntry->GetRoundOffExp(playerLevel);
		if (iExp)
		{
			pkPlayer->AddEXP(iExp, SYS_FUNC_TP_TASK_FINISH, pkEntry->_iTaskType);
		}

		//策划要求：每日任务的终极经验奖励，用指挥官的等级计算经验奖励
		attr_value_type iBuddyExp = pkEntry->GetRoundOffBuddyExp(playerLevel);
		if (iBuddyExp)
		{
			pkPlayer->AddExpToActivateBuddy(iBuddyExp, SYS_FUNC_TP_TASK_FINISH, pkEntry->_iTaskType);
		}
		

		MoneyType iCash = pkEntry->GetRoundOffCash(playerLevel);
		if (iCash)
		{
			pkPlayer->AddCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_TASK_FINISH, pkEntry->_iTaskType);
		}

		attr_value_type iSp = pkEntry->GetRoundOffSp(playerLevel);
		if (iSp)
		{
			pkPlayer->AddSkillLevelupPoints(iSp, SYS_FUNC_TP_TASK_FINISH, pkEntry->_iTaskType);
		}

		MoneyType iGuild = pkEntry->GetRoundOffGuild(playerLevel);
		if (iGuild)
		{
			pkPlayer->AddGuildMoney(iGuild, SYS_FUNC_TP_TASK_FINISH, pkEntry->_iTaskType);
		}
		
		if (!pkEntry->_kRoundOffItems.empty())
		{
			UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkEntry->_kRoundOffItems, pkPlayer, SYS_FUNC_TP_TASK_FINISH, pkEntry->_iTaskType, pkEntry->GetMailTitle(), pkEntry->GetMailDesc());
		}

		pkTaskAssignInfo->SetRewarded(iTaskAssignType, true);

	}while(0);

	return rslt;
}
ResultType LC_ServerTaskMap::Phagotrophy(LC_Task* pkPhagotrophTask, LC_Task* pkPhagocytableTask, float propo)
{
	if (pkPhagotrophTask==pkPhagocytableTask || NULL==pkPhagotrophTask || NULL==pkPhagocytableTask)
	{
		return RE_FAIL;
	}

	LC_ProcessingTaskEntry* pkPhagotrophEntry = GetProcessingTaskEntryByID(pkPhagotrophTask->GetTaskID());
	LC_ProcessingTaskEntry* pkPhagocytableEntry = GetProcessingTaskEntryByID(pkPhagocytableTask->GetTaskID());

	if (NULL==pkPhagotrophEntry || NULL==pkPhagocytableEntry)
	{
		return RE_FAIL;
	}

	int iExpA = pkPhagotrophTask->ComputeExpBonus(pkPhagotrophEntry, m_pkOwner, true);
	int iCashA = pkPhagotrophTask->ComputeUnBindGoldBonus(pkPhagotrophEntry, m_pkOwner, true);
	int iRuneCreamA = pkPhagotrophTask->ComputeRuneCreamBonus(pkPhagotrophEntry, m_pkOwner, true);

	double dExpB = pkPhagocytableTask->ComputeExpBonus(pkPhagocytableEntry, m_pkOwner) * propo;
	double dCashB = pkPhagocytableTask->ComputeUnBindGoldBonus(pkPhagocytableEntry, m_pkOwner) * propo;
	double dRuneCreamB = pkPhagocytableTask->ComputeRuneCreamBonus(pkPhagocytableEntry, m_pkOwner) * propo;

	LC_TaskParams& kParams = pkPhagotrophEntry->GetTaskParam();

	kParams.SetExpPro((int)(dExpB*10000/UT_MathBase::MaxInt(iExpA, 1)) + kParams.GetExpPro());
	kParams.SetCashPro((int)(dCashB*10000/UT_MathBase::MaxInt(iCashA, 1)) + kParams.GetCashPro());
	kParams.SetRuneCreamPro((int)(dRuneCreamB*10000/UT_MathBase::MaxInt(iRuneCreamA, 1)) + kParams.GetRuneCreamPro());

	((LC_ServerPlayer*)m_pkOwner)->CancelTask(pkPhagocytableTask->GetTaskID());


	int32_t iTaskType = pkPhagotrophTask->GetTaskType();

	LC_TaskTypeData* pkTypeData = GetTypeData(iTaskType);
	if (pkTypeData)
	{
		uint32_t timestamp = uint32_t(GET_CURRENT_TIMESTAMP_IN_MILISECONDS()/1000);
		pkTypeData->OnTaskPhagotrophy(pkPhagotrophTask, pkPhagocytableTask, timestamp);
		UpdateDirtyFlag(DIRTY_FLAG_TASK_TYPE_DATA);
	}

	UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);

	return RE_SUCCESS;
}

void LC_ServerTaskMap::GenerateTaskItemFlopInfoOnNPCDead( LC_ItemFlopInfoList& rkFlopInfoList, LC_ServerNPC* pkNPC, LC_TaskPtrs& kTaskPtrs)
{
	
	LC_TaskPtrs::iterator it = kTaskPtrs.begin();
	for (; it!=kTaskPtrs.end(); ++it)
	{
		LC_ServerTask* pkTask =  (LC_ServerTask*)(*it);
		LC_ProcessingTaskEntry* pkEntry = GetProcessingTaskEntryByID(pkTask->GetTaskID());
		if (pkEntry && RE_TASK_FINISH_SUCCESS != pkTask->CanFinish(m_pkOwner, GET_PROCESS_TIME))
		{
			pkTask->GenerateTaskItemFlopInfoOnNPCDead(pkNPC, m_pkOwner, pkEntry, rkFlopInfoList);
		}
	}
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"GetTaskKillMonsterItem End");
}

void LC_ServerTaskMap::InitBillboardTaskList()
{
	LC_TaskManager* pkTaskManager = LC_TaskManager::GetSingletonPtr();
	if (NULL == pkTaskManager)
		return;

	LC_TaskManager::BillBoardNPCTypeList& rList = pkTaskManager->GetBillBoardNPCTypeList();
	for (int i = 0;i < (int)rList.size();++i)
	{
		LC_TaskMap::InitBillboardTaskList(rList[i]);
	}
}

void LC_ServerTaskMap::PBLoadProcessingTaskInfo(const char_data_define::processing_task_asset_info& info)
{
	LC_TaskManager* pkTaskMgr = LC_TaskManager::GetSingletonPtr();

	for(int i = 0; i < info.infos_size(); ++i)
	{
		const char_data_define::processing_task_entry& entry = info.infos(i);
		LC_ProcessingTaskEntry& kEntry = m_kProcessingTaskMap[entry.taskid()];

		if (NULL == pkTaskMgr->GetTask(entry.taskid()))
		{
			continue;
		}

		kEntry.GetDataFromPB(entry);
	}
}

void LC_ServerTaskMap::PBSaveProcessingTaskInfo(char_data_define::processing_task_asset_info& info)
{
	LC_ProcessingTaskEntryMap::iterator it = m_kProcessingTaskMap.begin();

	for(; it!=m_kProcessingTaskMap.end(); ++it)
	{
		LC_ProcessingTaskEntry& kEntry = it->second;
		if(kEntry.GetTaskID()>0)
		{
			char_data_define::processing_task_entry * entry = info.mutable_infos()->Add();
			kEntry.SetDataToPB(*entry);
		}
	}
}

void LC_ServerTaskMap::PBLoadFailTaskInfo(const char_data_define::fail_task_asset_info& info)
{
	for (int i = 0; i < info.infos_size(); ++i)
	{
		const char_data_define::fail_task_entry& entry = info.infos(i);
		LC_FailTaskEntry& kEntry = m_kFailTaskMap[entry.task_id()];
		kEntry.SetTaskID(entry.task_id());
		kEntry.SetTaskIndex(entry.task_index());
	}
}

void LC_ServerTaskMap::PBSaveFailTaskInfo(char_data_define::fail_task_asset_info& info)
{
	LC_FailTaskEntryMap::iterator it = m_kFailTaskMap.begin();

	for(; it!=m_kFailTaskMap.end(); ++it)
	{
		LC_FailTaskEntry& kEntry = it->second;
		char_data_define::fail_task_entry * entry = info.mutable_infos()->Add();
		entry->set_task_id(kEntry.GetTaskID());
		entry->set_task_index(kEntry.GetTaskIndex());
	}
}

void LC_ServerTaskMap::PBLoadTaskChapterRewardInfo(const ::google::protobuf::RepeatedField<::google::protobuf::uint32>& info)
{
	for(int i=0; i<info.size(); ++i)
	{
		uint32_t id = info.Get(i);
		m_nChapterRewordIDs.insert(id);
	}
}

void LC_ServerTaskMap::PBSaveTaskChapterRewardInfo(::google::protobuf::RepeatedField<::google::protobuf::uint32>& info)
{
	for(mem::set<int32_t>::iterator it=m_nChapterRewordIDs.begin(); it!=m_nChapterRewordIDs.end(); ++it)
	{
		uint32_t* id = info.Add();
		*id = *it;
	}
}
